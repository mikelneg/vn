#ifndef UERIIOWIWGGGAEF_VN_UTILITIES_H_
#define UERIIOWIWGGGAEF_VN_UTILITIES_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              
    
    namespace vn 

    Various utilities..
-----------------------------------------------------------------------------*/

#include <cstddef>
#include <type_traits>

namespace vn {        

    template <typename>         
    using void_type = void;        

    namespace detail {            
        template <typename T>
        struct identity_ { using type = T; };
            
        template <typename T>   // used to postpone the triggering of static_asserts
        constexpr bool false_() noexcept { return false; }
        
        template <std::size_t N>
        struct index_ { constexpr static std::size_t value = N; 
                        constexpr operator std::size_t() const noexcept { return value; } };
        
        template <typename T, typename ...>
        struct index_of_helper_ {
            static_assert(false_<T>(), "static_assert triggered: T not found in Args in index_of<T,Args...>");
        };
            
        template <typename T, typename ...Targs>
        struct index_of_helper_<T,T,Targs...> {
            template <std::size_t N>
            using type = index_<N>;
        };
        
        template <typename T, typename H, typename ...Targs>
        struct index_of_helper_<T,H,Targs...> {
            template <std::size_t N>
            using type = typename index_of_helper_<T,Targs...>::template type<N+1>;
        };        
             
        template <template <typename> typename Pred, template <typename> typename Map, typename = void>
        struct conditional_remap_ { // chosen if typename Map<void>::type is invalid         
            template <typename Q>
            using applied_to = std::conditional_t<Pred<Q>::value, Map<Q>, Q>; 
        };

        template <template <typename> typename Pred, template <typename> typename Map>
        struct conditional_remap_<Pred, Map, void_type<typename Map<void>::type>> { // chosen if typename Map<void>::type is valid      
            template <typename Q>
            using applied_to = std::conditional_t<Pred<Q>::value, typename Map<Q>::type, Q>; 
        };                
    }    

    template <typename T>
    struct size_of_pack {
        static_assert(detail::false_<T>(), "static_assert triggered: T is not a variadic type.");
    };

    template <template <typename...> typename TT, typename ...Args>
    struct size_of_pack<TT<Args...>> {
        static constexpr std::size_t value = sizeof...(Args);
        constexpr operator std::size_t() const { return sizeof...(Args); }
    };

    template <typename T, typename ...TArgs>
    struct index_of {    
        constexpr static std::size_t value = detail::index_of_helper_<T,TArgs...>::template type<0>::value;
        constexpr operator std::size_t() const noexcept { return value; }
    };
    
    template <typename T, template <typename...> typename TT, typename ...TArgs>
    struct index_of<T, TT<TArgs...>> {    
        constexpr static std::size_t value = detail::index_of_helper_<T,TArgs...>::template type<0>::value;    
        constexpr operator std::size_t() const noexcept { return value; }
    };    

    template <template <typename...> typename TT, typename ...TArgs> // catches cases where TT<TArgs...> appears as first and second param,                                                                      
    struct index_of<TT<TArgs...>, TT<TArgs...>> {                    // in which case we do not dig into TArgs...
        constexpr static std::size_t value = 0;    
        constexpr operator std::size_t() const noexcept { return value; }
    };
    
    template <typename T>
    using remove_rvalue_reference = typename detail::conditional_remap_<std::is_rvalue_reference,std::remove_reference>::template applied_to<T>;
    
    template <typename T>
    using identity = typename detail::identity_<T>::type;

    //template <typename R, typename C, typename ...Args>
    //R return_type(R(C::*)(Args...));      
    //    
    //template <typename R, typename ...Args>
    //R return_type(R(*)(Args...));
    //
    //namespace detail {    
    //       
    //    template <int...Ns>
    //    struct indx {
    //        template <template <int...> typename TT>
    //    	using into = TT<Ns...>;
    //        
    //        template <template <typename, int...> typename TT, typename T>
    //        using into_with = TT<T,Ns...>;
    //    };
    //    
    //    template <int...Ns>
    //    struct rev_;
    //    
    //    template <>
    //    struct rev_<> {
    //    	template <int...Ps>
    //    	using type = indx<Ps...>;
    //    };
    //    
    //    template <int N, int...Ns> 
    //    struct rev_<N,Ns...> {
    //    	template <int...Ps>
    //    	using type = typename rev_<Ns...>::template type<N,Ps...>;
    //    };
    //            
    //    template <typename T>
    //    struct rev_h; 
    //    
    //    template <template <int...> typename TT, int...Ns>
    //    struct rev_h<TT<Ns...>> {
    //    	using type = typename rev_<Ns...>::template type<>::template into<TT>;
    //    };
    //    
    //    template <template <typename, int...> typename TT, typename T, int...Ns>
    //    struct rev_h<TT<T,Ns...>> {
    //        using type = typename rev_<Ns...>::template type<>::template into_with<TT,T>;
    //    };    
    //}

} // namespace
#endif

