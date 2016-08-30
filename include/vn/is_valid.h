#ifndef TOWIWOIWJOPAJIG_VN_IS_VALID_H_
#define TOWIWOIWJOPAJIG_VN_IS_VALID_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                                  
    
    namespace vn

    is_valid(...)

-----------------------------------------------------------------------------*/

namespace vn {

    namespace detail {
    
        template <typename T> 
        struct is_valid_ : T {                        
            struct priv_t;            
                                   
            using T::operator();            
            priv_t operator()(...) const; 
                
        };

        template <typename T>
        struct check_valid_ {        
            template <typename...Qs>
            constexpr bool operator()(Qs&&...qs) const {
                return !std::is_same<decltype((std::declval<T const&>())(std::forward<Qs>(qs)...)),typename T::priv_t>::value;
            }
            
            template <typename...Qs>
            constexpr bool with_param_types() const {
                return !std::is_same<decltype((std::declval<T const&>())(std::declval<Qs>()...)),typename T::priv_t>::value;
            }                        
        };

    }
    

    template <typename F>
    constexpr auto is_valid(F f) {
        return detail::check_valid_<detail::is_valid_<F>>{}; 
    }



} // namespaces
#endif