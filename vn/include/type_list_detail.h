#ifndef YTUIWOWFOOJAF_VN_TYPE_LIST_DETAIL_H_
#define YTUIWOWFOOJAF_VN_TYPE_LIST_DETAIL_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)

    Implementation details for vn::type_list -- shouldn't be used directly..    
-----------------------------------------------------------------------------*/

#include <type_traits> // for std::true_type/std::false_type

namespace vn {
                
    template <typename ...Args>
    struct type_list;
    
    namespace detail {

        template <typename ...Args>
        struct discharge_ {
            template <template <typename...> typename TT>
            using into = TT<Args...>;
        };

        template <typename ...Args, template <typename...> typename XX>
        struct discharge_<XX<Args...>> {
            template <template <typename...> typename TT>
            using into = TT<Args...>;
        };

        inline namespace filter {
            template <typename, typename ...Args> 
            struct filter_;
                    
            template <typename F, typename H, typename ...Args> 
            struct filter_<F,H,Args...> {   
                template <typename ...TArgs>
                using type = typename filter_<F,Args...>::template type<TArgs...,H>;
            };
            
            template <typename F, typename ...Args>    
            struct filter_<F,F,Args...> {
                template <typename ...TArgs>
                using type = typename filter_<F,Args...>::template type<TArgs...>;      
            };
            
            template <typename T>    
            struct filter_<T> {
                template <typename ...TArgs>
                using type = type_list<TArgs...>;    
            };
            
            template <typename ...Args>    // compatibility_helper used to avoid ICE in msvc...
            struct compatibility_helper_filter_ {
                using type = typename filter_<Args...>::template type<>;
            };
            
            template <typename ...Args> // generalizes filter_<> to work with none-or-many filter types
            struct filter_list_;
              
            template <>
            struct filter_list_<> {
                template <typename... TArgs>
                using type = type_list<TArgs...>;
            };                    

            template <typename T, typename ...Args> 
            struct filter_list_<T,Args...> {
                template <typename ...TArgs>
                using type = typename discharge_<typename compatibility_helper_filter_<T,TArgs...>::type>::template into<typename filter_list_<Args...>::template type>;                            
            };            
            
            template <typename F, typename L>    // compatibility_helper used to avoid ICE in msvc... 
            struct compatibility_helper_filter_list_ {
                using type = typename discharge_<L>::template into<typename discharge_<F>::template into<filter_list_>::template type>;
            };

        }

        inline namespace unique {
            template <typename T>
            struct unique_;
            
            template <typename T, typename ...Args>
            struct unique_<type_list<T,Args...>> {
                template <typename ...TArgs>
                using type = typename unique_<typename filter_list_<T>::template type<Args...>>::template type<TArgs...,T>;                
            };        
            
            template <>
            struct unique_<type_list<>> {
                template <typename ...TArgs>
                using type = type_list<TArgs...>;
            };

            template <typename ...Args>
            struct compatibility_helper_unique_ {
                using type = typename unique_<type_list<Args...>>::template type<>;
            };
        }

        inline namespace merge {      
            template <typename ...>
            struct merge_lists_;

            template <typename ...TArgs>
            struct merge_lists_<type_list<TArgs...>> {
                using type = type_list<TArgs...>;
            };

            template <typename ...TArgs, typename ...RArgs>
            struct merge_lists_<type_list<TArgs...>, type_list<RArgs...>> {
                using type = type_list<TArgs..., RArgs...>;
            };

            template <typename ...TArgs, typename ...RArgs, typename ...Args>
            struct merge_lists_<type_list<TArgs...>, type_list<RArgs...>, Args...> {
                using type = typename merge_lists_< type_list<TArgs..., RArgs...>, Args...>::type;
            };
                   
            template <typename ...Args>
            struct merge_ {                         
                using type = typename merge_lists_<typename discharge_<Args>::template into<type_list>... >::type;
            };                      
        }

        inline namespace apply_op {            
            template <template <typename> typename TT, typename PairwiseOp, typename ...Args>
            struct apply_;
            
            template <template <typename> typename TT, typename PairwiseOp, typename T, typename H, typename P, typename ...Args>
            struct apply_<TT,PairwiseOp,T,H,P,Args...> {
                using result = decltype(PairwiseOp{}( TT<T>{}, typename apply_<TT,PairwiseOp,H,P,Args...>::result{} ));
            };
            
            template <template <typename> typename TT, typename PairwiseOp, typename T, typename H>
            struct apply_<TT,PairwiseOp,T,H> {
                using result = decltype(PairwiseOp{}( TT<T>{}, TT<H>{} ));
            };
            
            template <template <typename> typename TT, typename PairwiseOp, typename T>
            struct apply_<TT,PairwiseOp,T> {
                using result = TT<T>;
            };
            
            template <typename Pred, typename ONE = std::true_type, typename ZERO = std::false_type>
            struct bool_op {
                template <typename T, typename R>
                std::enable_if_t<(Pred{}(T{},R{})), ONE> operator()(T,R) const;
            
                template <typename T, typename R>
                std::enable_if_t<!(Pred{}(T{},R{})), ZERO> operator()(T,R) const;
            };
            
            struct both {
                template <typename T, typename R>
                constexpr bool operator()(T,R) const { return T::value && R::value; }
            };
            
            struct either {
                template <typename T, typename R>
                constexpr bool operator()(T,R) const { return T::value || R::value; }
            };
                    
            struct if_then {
                template <typename T, typename R>
                constexpr bool operator()(T,R) const { return !T::value || R::value; }
            };
            
            struct only_one {
                template <typename T, typename R>
                constexpr bool operator()(T,R) const { return !(T::value && R::value) && (T::value || R::value); }
            };
                    
            using conj = bool_op<both>;
            using disj = bool_op<either>;
            using impl = bool_op<if_then>;
            using xor_ = bool_op<only_one>;
            
            template <template <typename> typename TT, typename PairwiseOp>
            struct apply_op_ {
                template <typename ...Args>
                using to = typename apply_<TT,PairwiseOp,Args...>::result;
            };
        }
    } // namespace detail
} // namespace vn
#endif
