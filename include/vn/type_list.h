#ifndef OQQQWEQEFVXCBBNNCC_VN_TYPE_LIST_H_
#define OQQQWEQEFVXCBBNNCC_VN_TYPE_LIST_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)

    namespace vn
    
    template <typename ...Args> struct type_list;    

    - type_list<...> supports various operations, including filter, join, 
      merge, unique, discharge, and predicate satisfaction checks

      examples:

      type_list<A,B,C>::filter<A,B> == type_list<C>
      type_list<A,B,C>::join<D,type_list<E>> == type_list<A,B,C,D,type_list<E>>
      type_list<A,B,C,C,B,A,D>::unique == type_list<A,B,C,D>    
      type_list<A,B,C>::merge<D,E,F,type_list<H,I,J>> == type_list<A,B,C,D,E,F,H,I,J>
      type_list<A,B,C>::discharge<std::tuple> == std::tuple<A,B,C>
      type_list<int,float>::each_satisfies<std::is_scalar> == std::true_type
      type_list<int,float>::any_satisfies<std::is_placeholder> == std::false_type

-----------------------------------------------------------------------------*/
#include <vn/detail/type_list_detail.h>
#include <type_traits>

namespace vn {

    template <typename ...Args>
    struct type_list {               
                              
        template <typename ...TArgs>   
        using filter = typename detail::compatibility_helper_filter_list_<type_list<TArgs...>,type_list<Args...>>::type;        

        template <typename ...TArgs>    // join appends any type, including type_lists
        using join = type_list<Args...,TArgs...>;
    
        using unique = typename detail::compatibility_helper_unique_<Args...>::type;    
        
        template <typename ...TArgs>    // merge appends, but flattens type_lists 
        using merge = typename detail::merge_<type_list<Args...>,TArgs...>::type;
        
        template <template <typename...> typename TT>
        using discharge = TT<Args...>;
        
        // below evaluate to std::true_type or std::false_type
        template <template <typename> typename PredicateTT>
        using each_satisfies = typename detail::apply_op_<PredicateTT,detail::conj>::template to<Args...>; 
        
        template <template <typename> typename PredicateTT>
        using any_satisfies = typename detail::apply_op_<PredicateTT,detail::disj>::template to<Args...>;  

        template <template <typename> typename PredicateTT>
        using one_satisfies = typename detail::apply_op_<PredicateTT,detail::xor_>::template to<Args...>;  
    };    

    template <template <typename...> typename TT, typename ...Args>
    type_list<Args...> type_list_from(TT<Args...>); 
 
} // namespace
#endif
