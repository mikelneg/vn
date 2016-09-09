#ifndef HGHGJKWLFWWFFAS_VN_BOOST_UTILITIES_H_
#define HGHGJKWLFWWFFAS_VN_BOOST_UTILITIES_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              

    namespace vn

    Some utilities for Boost libraries:
    
    - std::vector<...> make_variant_vector<AdditionalTypes>(Args&&...args)
    - std::vector<...,Allocator> make_variant_vector_with_allocator(Allocator&&,Args&&...args)
        - forwards args into a std::vector<boost::variant<AdditionalTypes...,Args...>> (with unique AdditionalTypes+Args)
          and returns it        

    - make_lambda_visitor<ReturnType>(Funcs&&...) (to be used with boost::apply_visitor)

    - boost_hasher (a vn::hasher<> variation that uses boost::hash_combine)

    - extended_variant<...> template, accepts boost::variant<...> types and generates an 
      "extended" variant type that includes each sub-type

    - variant_over<...> template, accepts any set of types and generates a boost::variant<...>
      over them; additionally "flattens" any boost::variant<...> types 
      (e.g., variant_over<int,char,boost::variant<float,double>> == boost::variant<int,char,float,double>)

    - mpl_sequence_to_list<...> converts a boost mpl sequence to a vn::type_list<...>
    
    - zip_mpl_sequences<...> "zips" a sequence of mpl sequences into a vn::type_list<...> 

-----------------------------------------------------------------------------*/
#include <cstddef>

#include <vn/hasher.h>
#include <vn/type_list.h>
#include <vn/utilities.h>

#include <type_traits>
#include <utility>
#include <vector>

#include <boost/functional/hash.hpp>
#include <boost/mpl/begin_end.hpp>
#include <boost/variant.hpp>

#include <vn/composite_function.h>

namespace vn {

template <typename... AdditionalTypes, typename... Args>
auto make_variant_vector(Args&&... args)
{
    using ret_var = typename type_list<std::remove_reference_t<AdditionalTypes>..., std::remove_reference_t<Args>...>::unique::template discharge<boost::variant>;
    std::vector<ret_var> v;
    v.reserve(sizeof...(Args));
    int discard[]{ 0, (v.emplace_back(std::forward<Args>(args)), 0)... };
    (void)discard;
    return v;
}

// // lambda_visitor and company replaced with composite_function
//
//template <typename ...AdditionalTypes, typename Allocator, typename ...Args>
//auto make_variant_vector_with_allocator(Allocator&& allocator, Args&&...args) {
//    using ret_var = typename type_list<std::remove_reference_t<AdditionalTypes>...,std::remove_reference_t<Args>...>::unique::template discharge<boost::variant>;
//    std::vector<ret_var, typename Allocator::template rebind<ret_var>::other> v;
//    v.reserve(sizeof...(Args));
//    int discard[]{0,(v.emplace_back(std::forward<Args>(args)),0)...};
//    (void)discard;
//    return v;
//}
//
//template <typename R, typename ...Args>
//struct lambda_visitor : Args..., boost::static_visitor<R> {
//    template <typename ...CArgs>
//    lambda_visitor(CArgs&&... cargs) : Args(std::forward<CArgs>(cargs))... {}
//
//    lambda_visitor(lambda_visitor&&) = default;
//    lambda_visitor(lambda_visitor const&) = default;
//    lambda_visitor& operator=(lambda_visitor const&) = delete;
//};
//
//template <typename R = void, typename ...Fs>
//auto make_lambda_visitor(Fs&&... fs) {
//    return lambda_visitor<R, typename std::remove_reference<Fs>::type...>{std::forward<Fs>(fs)...};
//}
//

template <typename V>
struct lambda_visitor : V {
    template <typename... Ts>
    constexpr lambda_visitor(Ts&&... ts) noexcept(noexcept(V(std::forward<Ts>(ts)...)))
        : V(std::forward<Ts>(ts)...)
    {
    }

    template <typename T>
    decltype(auto) operator()(T const& t) const
    {
        using boost::apply_visitor;
        return apply_visitor(static_cast<V const&>(*this), t);
    }
};

template <typename... Fs>
constexpr auto make_lambda_visitor(Fs&&... fs)
{
    using comp_func_type = decltype(vn::make_composite_function(std::forward<Fs>(fs)...));
    return lambda_visitor<comp_func_type>{ std::forward<Fs>(fs)... };
}

//template <typename...Fs>
//constexpr decltype(auto) make_lambda_visitor(Fs&&...fs) {
//    return vn::make_composite_function(std::forward<Fs>(fs)...);
//}

namespace detail {
    struct boost_hash_combine_functor {
        template <typename T>
        void operator()(std::size_t& s, T const& t) const { ::boost::hash_combine(s, t); }
    };
}

// using boost_hasher = vn::hasher<detail::boost_hash_combine_functor>; // previously used a typedef.
struct boost_hasher : vn::hasher<detail::boost_hash_combine_functor> { // the typedef was generating many annoying "decorated name length exceeded" warnings..
    using vn::hasher<detail::boost_hash_combine_functor>::hasher; // now we just inherit from it and use its ctors.
};

namespace detail {
    template <typename It, typename E>
    struct extract_MPL_sequence_ {
        template <typename... Args>
        using type = typename extract_MPL_sequence_<typename boost::mpl::next<It>::type, E>::template type<Args..., typename boost::mpl::deref<It>::type>;
    };

    template <typename E>
    struct extract_MPL_sequence_<E, E> {
        template <typename... Args>
        using type = vn::type_list<Args...>;
    };

    template <typename T>
    struct mpl_sequence_to_list_ {
        using type = typename extract_MPL_sequence_<typename boost::mpl::begin<T>::type, typename boost::mpl::end<T>::type>::template type<>;
    };

    template <typename... Args>
    struct mpl_sequence_to_list_<boost::variant<Args...> > {
        using var_type = typename boost::variant<Args...>::types;
        using type = typename extract_MPL_sequence_<typename boost::mpl::begin<var_type>::type, typename boost::mpl::end<var_type>::type>::template type<>;
    };
}

template <typename T>
using mpl_sequence_to_list = typename detail::mpl_sequence_to_list_<T>::type;

template <typename... Args>
using zip_mpl_sequences = typename vn::type_list<>::merge<typename detail::mpl_sequence_to_list_<Args>::type...>;

namespace detail {
    template <typename... Args> // Args must be boost::variant<...> types
    struct common_variant_ {
        using type = typename zip_mpl_sequences<Args...>::unique::template discharge<boost::variant>;
    };

    template <typename T>
    struct list_from_T_or_variant_ {
        using type = vn::type_list<T>;
    };

    template <typename... Args>
    struct list_from_T_or_variant_<boost::variant<Args...> > {
        using type = typename detail::mpl_sequence_to_list_<boost::variant<Args...> >::type;
    };

    template <typename... Args> // Args may be any T or boost::variant<...> types; boost::variant<...> types are flattened
    struct variant_over_ {
        using type = typename vn::type_list<>::template merge<typename list_from_T_or_variant_<Args>::type...>::unique::template discharge<boost::variant>;
    };
}

template <typename VarType, typename T>
using variant_contains = vn::contains<mpl_sequence_to_list<VarType>, T>;

template <typename... Args> // might be redundant given variant_over<>...
using extended_variant = typename detail::common_variant_<Args...>::type; // Args must be boost::variant<...> types

template <typename... Args>
using variant_over = typename detail::variant_over_<Args...>::type; // Args may be any T or boost::variant<...> types; boost::variant<...> types are flattened

} // namespace
#endif
