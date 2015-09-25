#ifndef YTUIWOWFOOJAF_VN_TYPE_LIST_DETAIL_H_
#define YTUIWOWFOOJAF_VN_TYPE_LIST_DETAIL_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)

    Implementation details for type_list -- shouldn't be used directly..    
-----------------------------------------------------------------------------*/

#include <type_traits> // for std::true_type/std::false_type

namespace vn {
                
    template <typename ...Args>
    struct type_list;
    
    namespace detail {

        template <typename T, typename ...Args>
        struct filter_;
        
        template <typename T, typename H, typename ...Args>
        struct filter_<T,H,Args...> {
            template <typename ...TArgs>
            using type = typename filter_<T,Args...>::template type<TArgs...,H>;
        };
        
        template <typename T, typename ...Args>
        struct filter_<T,T,Args...> {
            template <typename ...TArgs>
            using type = typename filter_<T,Args...>::template type<TArgs...>;
        };
        
        template <typename T> 
        struct filter_<T> {
            template <typename ...TArgs>
            using type = type_list<TArgs...>;    
        };
        
        template <typename T>
        struct unique_;
        
        template <typename T, typename ...Args>
        struct unique_<type_list<T,Args...>> {
            template <typename ...TArgs>
            using type = typename unique_<typename type_list<Args...>::template filter<T>>::template type<TArgs...,T>;
        };
        
        template <>
        struct unique_<type_list<>> {
            template <typename ...TArgs>
            using type = type_list<TArgs...>;
        };
      
        template <typename T, typename ...>
        struct merge_;
                
        template <typename ...TArgs, typename ...RArgs>
        struct merge_<type_list<TArgs...>, type_list<RArgs...>> {
            using type = type_list<TArgs..., RArgs...>;
        };

        template <typename ...TArgs, typename ...RArgs, typename ...Args>
        struct merge_<type_list<TArgs...>, type_list<RArgs...>, Args...> {
            using type = typename merge_< type_list<TArgs..., RArgs...>, Args...>::type;
        };

        template <typename T>
        struct flatten_ { 
            template <template <typename...> typename TT> 
            using type = TT<T>;
        };

        template <typename ...Args>
        struct flatten_<type_list<Args...>> {            
            template <template <typename...> typename TT>
            using type = TT<Args...>;
        };
        
        template <typename ...Args>
        struct join_flattened_ {         
            template <typename L>
            using type = typename L::template merge< typename merge_<typename flatten_<Args>::template type<type_list>...>::type >;
        };       
        
        template <typename ...Args>         // compatibility_helper's are needed to block ICEs in msvc
        struct compatibility_helper_filter_ {
            using type = typename filter_<Args...>::template type<>;
        };
        
        template <typename ...Args>
        struct compatibility_helper_unique_ {
            using type = typename unique_<type_list<Args...>>::template type<>;
        };

        // apply tools 
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

    } // namespace detail
} // namespace vn
#endif
