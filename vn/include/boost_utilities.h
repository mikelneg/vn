#ifndef HGHGJKWLFWWFFAS_VN_BOOST_UTILITIES_H_
#define HGHGJKWLFWWFFAS_VN_BOOST_UTILITIES_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              

    namespace vn

    Utilities for Boost libraries
    
    - make_variant_vector(Args...) 
        * moves/copies args into std::vector<boost::variant<Args...>> (with unique Args)
        * requires copy-initializable types 

    - make_lambda_visitor<ReturnType>(Funcs&&...)

-----------------------------------------------------------------------------*/
#include <cstddef>

#include "type_list.h"
#include "utilities.h"
#include "hasher.h"

#include <type_traits>
#include <utility>
#include <vector>

#include <boost/variant.hpp>
#include <boost/functional/hash.hpp>
#include <boost/mpl/begin_end.hpp>

namespace vn {    
    template <typename ...AdditionalTypes, typename ...Args>
    auto make_variant_vector(Args&&...args) {
        using ret_var = typename type_list<std::remove_reference_t<AdditionalTypes>..., 
                                           std::remove_reference_t<Args>...>::unique::template discharge<boost::variant>;
        std::vector<ret_var> v;
        v.reserve(sizeof...(Args));        
        int discard[]{0,(v.emplace_back(std::forward<Args>(args)),0)...};
        return v;
    }

    template <typename ...AdditionalTypes, typename Allocator, typename ...Args>
    auto make_variant_vector_with_allocator(Allocator&& allocator, Args&&...args) {
        using ret_var = typename type_list<std::remove_reference_t<AdditionalTypes>..., 
                                           std::remove_reference_t<Args>...>::unique::template discharge<boost::variant>;
        std::vector<ret_var, typename Allocator::template rebind<ret_var>::other> v;
        v.reserve(sizeof...(Args));        
        int discard[]{0,(v.emplace_back(std::forward<Args>(args)),0)...};
        return v;
    }
      
    template <typename R, typename ...Args>
    struct lambda_visitor : Args..., boost::static_visitor<R> {
        template <typename ...CArgs>
        lambda_visitor(CArgs&&... cargs) : Args(std::forward<CArgs>(cargs))... {}
    };

    template <typename R = void, typename ...Fs>
    auto make_lambda_visitor(Fs&&... fs) {         
        return lambda_visitor<R, typename std::remove_reference<Fs>::type...>{std::forward<Fs>(fs)...};
    }    
    
    namespace detail {
        struct boost_hash_combine_functor {
            template <typename T>
            void operator()(std::size_t& s, T const& t) const { ::boost::hash_combine(s,t); }
        };                                                                                
    }

    using boost_hasher = vn::hasher<detail::boost_hash_combine_functor>;

    namespace detail {
        template <typename It, typename E>
        struct extract_MPL_sequence_ {
            template <typename ...Args>
            using type = typename extract_MPL_sequence_<typename boost::mpl::next<It>::type, E>::template type<Args..., typename boost::mpl::deref<It>::type>;
        };
        
        template <typename E>
        struct extract_MPL_sequence_<E,E> {
            template <typename ...Args>
            using type = vn::type_list<Args...>;
        };
        
        template <typename T>
        struct mpl_sequence_to_list_ {
            using type = typename extract_MPL_sequence_<typename boost::mpl::begin<T>::type, typename boost::mpl::end<T>::type>::template type<>;
        };
        
        template <typename ...Args>
        struct mpl_sequence_to_list_<boost::variant<Args...>> {
            using var_type = typename boost::variant<Args...>::types;
            using type = typename extract_MPL_sequence_<typename boost::mpl::begin<var_type>::type, typename boost::mpl::end<var_type>::type>::template type<>;
        };            
    } 
    
    template <typename T>
    using mpl_sequence_to_list = typename detail::mpl_sequence_to_list_<T>::type;
    
    template <typename ...Args>
    using zip_mpl_sequences = typename vn::type_list<>::merge<typename detail::mpl_sequence_to_list_<Args>::type...>;
     
    namespace detail {
        template <typename ...Args> // Args must be boost::variant<...> types
        struct common_variant_ {
            using type = typename zip_mpl_sequences<Args...>::unique::template discharge<boost::variant>;
        };     

        template <typename T>
        struct list_from_T_or_variant_{
            using type = vn::type_list<T>;
        };

        template <typename ...Args>
        struct list_from_T_or_variant_<boost::variant<Args...>> {
            using type = typename detail::mpl_sequence_to_list_<boost::variant<Args...>>::type;
        };

        template <typename ...Args> // Args may be any T or boost::variant<...> types; boost::variant<...> types are flattened
        struct variant_over_ { 
            using type = typename vn::type_list<>::template merge< typename list_from_T_or_variant_<Args>::type ... >::unique::template discharge<boost::variant>;                            
        };
    } 

    template <typename ...Args>
    using extended_variant = typename detail::common_variant_<Args...>::type; // Args must be boost::variant<...> types

    template <typename ...Args>
    using variant_over = typename detail::variant_over_<Args...>::type; // Args may be any T or boost::variant<...> types; boost::variant<...> types are flattened

} // namespace
#endif

