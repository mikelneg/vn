#ifndef OQQQWEQEFVXCBBNNCC_VN_TYPE_LIST_H_
#define OQQQWEQEFVXCBBNNCC_VN_TYPE_LIST_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)

    namespace vn
    
    template <typename ...Args> struct type_list;    

    - type_list<A,B,C,...> supports several "operations" on the list,
      including filter, join, unique, merge, discharge, and checks

      e.g., 
        type_list<A,B,C>::filter<A>          == type_list<B,C>
        type_list<A,B,C>::join<D,E>          == type_list<A,B,C,D,E>
        type_list<A,B,C,C,B,A,D>::unique     == type_list<A,B,C,D>    
        type_list<A,B,C>::merge<list<D,E,F>,list<H,I,J>> == type_list<A,B,C,D,E,F,H,I,J>
        type_list<A,B,C>::discharge<std::tuple> == std::tuple<A,B,C>
        type_list<int,float>::each_satisfies<std::is_scalar> == std::true_type

-----------------------------------------------------------------------------*/
#include "type_list_detail.h"

namespace vn {

    template <typename ...Args>
    struct type_list {
                                // we use a compatibility_helper_xxx here because, for whatever reason, 
        template <typename T>   // "typename detail::filter_<T,Args...>::template type<>;" causes an ICE in msvc..
        using filter = typename detail::compatibility_helper_filter_<T,Args...>::type; 
       
        template <typename ...TArgs>
        using join = type_list<Args...,TArgs...>;
    
        using unique = typename detail::compatibility_helper_unique_<Args...>::type;
    
        template <typename ...TArgs>
        using merge = typename detail::merge_<type_list<Args...>,TArgs...>::type;
        
        template <typename ...TArgs>
        using join_flattened = typename detail::join_flattened_<TArgs...>::template type< type_list<Args...> >;

        template <template <typename...> typename TT>
        using discharge = TT<Args...>;

        template <template <typename> typename PredicateTT>
        using each_satisfies = typename detail::apply_op_<PredicateTT,detail::conj>::template to<Args...>;   // == std::true_type or std::false_type

        template <template <typename> typename PredicateTT>
        using any_satisfies = typename detail::apply_op_<PredicateTT,detail::disj>::template to<Args...>;    // == std::true_type or std::false_type

        template <template <typename> typename PredicateTT>
        using one_satisfies = typename detail::apply_op_<PredicateTT,detail::xor_>::template to<Args...>;  // == std::true_type or std::false_type
    };    

    template <template <typename...> typename TT, typename ...Args>
    type_list<Args...> type_list_from(TT<Args...>); 
 
} // namespace
#endif
