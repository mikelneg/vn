#ifndef URUOWIOABVOWOIAF_VN_BOOST_COROUTINE_UTILITIES_H_
#define URUOWIOABVOWOIAF_VN_BOOST_COROUTINE_UTILITIES_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              
    
    namespace vn::coroutines:: 

    utilities for boost::coroutines
-----------------------------------------------------------------------------*/

// #include <boost/coroutine/asymmetric_coroutine.hpp>
#include <exception>

namespace vn {        
namespace coroutines {

    template <typename F>
    auto make_trycatch_coroutine(F func) 
    {
        return [func=std::move(func)](auto& yield) { 
            try {
                func(yield);
            } catch(boost::coroutines::detail::forced_unwind const&) { // this exception must propagate..
                throw; 
            } catch(...) { } // empty handler
        };
    }
    
    inline auto make_empty_coroutine() {
        return make_trycatch_coroutine([](auto& yield){ while (true) { yield(); } });
    }
    
    template <typename T, typename ...Args>
    void try_invoke(T& obj, Args&&...args) 
    {
        if (obj) {
            obj(std::forward<Args>(args)...);
        } else {
            throw std::runtime_error{__func__};
        }
    }

}} // namespace
#endif

