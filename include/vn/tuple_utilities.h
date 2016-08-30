#ifndef UTUOWOWPPQWE_VN_TUPLE_UTILITIES_H_
#define UTUOWOWPPQWE_VN_TUPLE_UTILITIES_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              

    namespace vn

    Utilities for std::tuple

-----------------------------------------------------------------------------*/
#include <cstddef>
#include <vn/utilities.h>
#include <tuple>
#include <type_traits>
#include <utility>

namespace vn {
    
    template <typename R, typename ...Args>
    std::tuple<Args...> 
    tuple_type_from_parameters(R(*)(Args...)); // no implementation 
    
    template <typename R, typename C, typename ...Args>
    std::tuple<Args...> 
    tuple_type_from_parameters(R(C::*)(Args...) const); // no implementation         
   
    namespace detail { 

        template <typename ...Args, std::size_t ...Ns, typename F>
        decltype(auto) call_with_all_elements_helper_(std::tuple<Args...> const& tuple_, std::index_sequence<Ns...>, F&& func)
        {
            return std::forward<F>(func)( std::get<Ns>(tuple_)... );        
        }

        template <typename ...Args, std::size_t ...Ns, typename F>
        void call_for_each_element_helper_(std::tuple<Args...> const& tuple_, std::index_sequence<Ns...>, F&& func) 
        {
            int discard[]{0,(std::forward<F>(func)(std::get<Ns>(tuple_)),0)... };
            (void)discard;
        }
    } 
    
    template <typename ...Args, typename F>
    decltype(auto) call_with_all_elements(std::tuple<Args...> const& tuple_, F&& func)        
    {
        return detail::call_with_all_elements_helper_(tuple_, std::index_sequence_for<Args...>{}, std::forward<F>(func));
    }

    template <typename ...Args, typename F>
    void call_for_each_element(std::tuple<Args...> const& tuple_, F&& func) 
    {
        detail::call_for_each_element_helper_(tuple_,std::index_sequence_for<Args...>{}, std::forward<F>(func));
    }
                
    template <typename ...Args, typename H>
    H // returns H& or H
    hash_tuple(std::tuple<Args...> const& tuple_, H&& hasher) 
    {        
        return call_with_all_elements(tuple_,std::forward<H>(hasher));
    }    

} // namespace
#endif

