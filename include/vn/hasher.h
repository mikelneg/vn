#ifndef TUWUWOOAPWOWOWEEFEF_VN_HASHER_H_
#define TUWUWOOAPWOWOWEEFEF_VN_HASHER_H_

/*-----------------------------------------------------------------------------
    Mikel Negugogor (http://github.com/mikelneg)                              

    namespace vn

    template <typename CombineFunctor>
    class hasher;    
    
        hasher(std::size_t seed = {});
        
        template <typename ...Args>
        hasher& operator()(Args const&...);

        Note:   CombineFunctor::operator() should have type
                    void operator()(std::size_t&, Arg const&) const

-----------------------------------------------------------------------------*/
#include <cstddef>

namespace vn {
    
    namespace detail {
        template <typename T>
        struct is_combine_functor_;
    }

    template <typename CombineFunctor>  // CombineFunctor must satisfy the requirements listed above
    class hasher {     static_assert(detail::is_combine_functor_<CombineFunctor>::value, 
                                      "static_assert triggered: CombineFunctor::operator() does not have the correct type.");                
        std::size_t seed{};    
    public:        
        constexpr explicit hasher(std::size_t seed) noexcept : seed{seed} {}                                    
        hasher() = default;
        hasher(hasher const&) = default;
        hasher& operator=(hasher const&) = default;

        template <typename ...Args>
        hasher& operator()(Args const&... args) {                                                    
            using C = CombineFunctor;            
            int discard[]{0,((C{}.*static_cast<void(C::*)(std::size_t&,decltype(args))const>(&C::operator()))(seed, args),0)...};                                                
            (void)discard;
            return *this;       // ugly static_cast of &C::operator() to guarantee that it has the right signature                          
        }
                
        constexpr bool operator==(hasher const& other) const noexcept { return seed == other.seed; }
        constexpr explicit operator std::size_t() const noexcept { return seed; }        
        friend constexpr std::size_t hash_value(hasher const& h) noexcept { return h.seed; }    
    };

    namespace detail {
        template <typename T>
        struct is_combine_functor_ {   
            using yes = char[1]; using no = char[2]; using no_ = char[3];    
            template <typename Q>    
            static yes& check( decltype(static_cast<void(Q::*)(std::size_t&,int const&) const>(&Q::operator())) );            
            template <typename Q>   // if both specialized "no" checks remain in lookup we get an ambiguity
            static no_& check( decltype(static_cast<void(Q::*)(std::size_t,int const&) const>(&Q::operator())) );            
            template <typename Q>
            static no& check( decltype(static_cast<void(Q::*)(std::size_t&&,int const&) const>(&Q::operator())) );    
            template <typename Q> 
            static no& check(...);    
            constexpr static bool value = sizeof(check<T>(0)) == sizeof(yes);            
        };        
    }
} // namespace
#endif

