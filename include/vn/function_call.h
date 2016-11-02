/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef IIEIWOAASDSA_VN_FUNCTION_CALL_H_
#define IIEIWOAASDSA_VN_FUNCTION_CALL_H_

/*-------------------------------------------------------------
    class function_call<...>;
    function_call<...> make_function_call(...);

    + hackish, not currently used
    - function_call<...> wraps a std::bind(...) object and stores a hash of
      the parameters
    - Uses default hasher vn::boost_hasher, which works with placeholder types
      and reference_wrapper<>
-------------------------------------------------------------*/

#include <vn/boost_variant_utilities.h>
#include <vn/hasher.h>
#include <vn/utilities.h>

#include <functional>
#include <type_traits>
#include <utility>

namespace vn {

template <typename...>
class function_call;

template <typename F, typename HashType>
class function_call<F, HashType> : private F {
    std::size_t parameter_hash;

public:
    function_call(F&& f, std::size_t parameter_hash) noexcept(noexcept(F{std::move(f)})) : F{std::move(f)}, parameter_hash{parameter_hash}
    {
    }

    function_call(function_call&&) = default;
    function_call(function_call const&) = default;

    function_call& operator=(function_call&&) = default;
    function_call& operator=(function_call const&) = default;

    using F::operator();

    template <typename... Args>
    constexpr bool calls_same_function_as(function_call<Args...> const&) const noexcept
    {
        return false;
    }
    constexpr bool calls_same_function_as(function_call const&) const noexcept
    {
        return true;
    }

    template <typename... Args>
    constexpr bool operator==(function_call<Args...> const&) const noexcept
    {
        return false;
    }
    constexpr bool operator==(function_call const& other) const noexcept
    {
        return parameter_hash == other.parameter_hash;
    }

    template <typename... Args>
    constexpr bool operator!=(function_call<Args...> const& other) const noexcept
    {
        return !(*this == other);
    }

    template <typename...>
    friend class function_call;
};

template <typename R, typename... Args, typename F, typename HashType>
class function_call<R (*)(Args...), F, HashType> : private F {
    using func_ptr_t = R (*)(Args...);

    std::size_t parameter_hash;
    func_ptr_t func_ptr;

public:
    function_call(F&& f, func_ptr_t func_ptr, std::size_t parameter_hash) noexcept(noexcept(F{std::move(f)}))
        : F{std::move(f)}, parameter_hash{parameter_hash}, func_ptr{func_ptr}
    {
    }

    function_call(function_call&&) = default;
    function_call(function_call const&) = default;

    function_call& operator=(function_call&&) = default;
    function_call& operator=(function_call const&) = default;

    using F::operator();

    template <typename... Args>
    constexpr bool calls_same_function_as(function_call<Args...> const&) const noexcept
    {
        return false;
    }

    template <typename XX, typename YY>
    constexpr bool calls_same_function_as(function_call<R (*)(Args...), XX, YY> const& other) const noexcept
    {
        return other.func_ptr == func_ptr;
    }

    template <typename... Args>
    constexpr bool operator==(function_call<Args...> const&) const noexcept
    {
        return false;
    }
    constexpr bool operator==(function_call const& other) const noexcept
    {
        return func_ptr == other.func_ptr && parameter_hash == other.parameter_hash;
    }
    template <typename... Args>
    constexpr bool operator!=(function_call<Args...> const& other) const noexcept
    {
        return !(*this).operator==(other);
    }

    template <typename...>
    friend class function_call;
};

template <typename R, typename C, typename... Args, typename F, typename HashType>
class function_call<R (C::*)(Args...), F, HashType> : private F {
    using func_ptr_t = R (C::*)(Args...);

    std::size_t parameter_hash;
    func_ptr_t func_ptr;

public:
    constexpr function_call(F&& f, func_ptr_t func_ptr, std::size_t parameter_hash) noexcept(noexcept(F{std::move(f)}))
        : F{std::move(f)}, parameter_hash{parameter_hash}, func_ptr{func_ptr}
    {
    }

    function_call(function_call&&) = default;
    function_call(function_call const&) = default;

    function_call& operator=(function_call&&) = default;
    function_call& operator=(function_call const&) = default;

    using F::operator();

    template <typename... Args>
    constexpr bool calls_same_function_as(function_call<Args...> const&) const noexcept
    {
        return false;
    }

    template <typename XX, typename YY>
    constexpr bool calls_same_function_as(function_call<R (C::*)(Args...), XX, YY> const& other) const noexcept
    {
        return other.func_ptr == func_ptr;
    }

    template <typename... Args>
    constexpr bool operator==(function_call<Args...> const&) const noexcept
    {
        return false;
    }
    constexpr bool operator==(function_call const& other) const noexcept
    {
        return func_ptr == other.func_ptr && parameter_hash == other.parameter_hash;
    }
    template <typename... Args>
    constexpr bool operator!=(function_call<Args...> const& other) const noexcept
    {
        return !(*this).operator==(other);
    }

    template <typename...>
    friend class function_call;
};

template <typename... Args, typename... BArgs>
constexpr bool calls_same_function(function_call<Args...> const& lhs, function_call<BArgs...> const& rhs) noexcept(noexcept(lhs.calls_same_function_as(rhs)))
{
    return lhs.calls_same_function_as(rhs);
}

namespace detail {
    template <typename T, bool Condition = (std::is_placeholder<T>::value > 0)>
    struct filter_placeholders;

    template <typename T>
    struct filter_placeholders<T, false> {
        template <typename Q>
        constexpr Q const& operator()(Q const& q) const noexcept
        {
            return q;
        }

        template <typename Q> // std::reference_wrapper<> types are common in std::bind scenarios, so we check for those and hash their object
        constexpr Q const& operator()(std::reference_wrapper<Q> const& q) const noexcept
        {
            return q.get();
        }
    };

    template <typename T>
    struct filter_placeholders<T, true> {
        constexpr auto operator()(T const&) const noexcept
        {
            return std::is_placeholder<T>::value;
        }
    };
}

template <typename Hasher = vn::boost_hasher, typename F, typename... Args>
auto make_function_call(F&& func, Args&&... args)
{
    std::size_t parameter_hash{Hasher{}(detail::filter_placeholders<typename std::remove_reference<Args>::type>{}(args)...)};
    return function_call<decltype(std::bind(std::forward<F>(func), std::forward<Args>(args)...)), Hasher>{
        std::bind(std::forward<F>(func), std::forward<Args>(args)...), parameter_hash};
}

template <typename Hasher = vn::boost_hasher, typename R, typename... PArgs, typename... Args>
auto make_function_call(R (*func_ptr)(PArgs...), Args&&... args)
{
    std::size_t parameter_hash{Hasher{}(detail::filter_placeholders<typename std::remove_reference<Args>::type>{}(args)...)};
    return function_call<decltype(func_ptr), decltype(std::bind(func_ptr, std::forward<Args>(args)...)), Hasher>{
        std::bind(func_ptr, std::forward<Args>(args)...), func_ptr, parameter_hash};
}

template <typename Hasher = vn::boost_hasher, typename R, typename C, typename... PArgs, typename... Args>
auto make_function_call(R (C::*func_ptr)(PArgs...), Args&&... args)
{
    std::size_t parameter_hash{Hasher{}(detail::filter_placeholders<typename std::remove_reference<Args>::type>{}(args)...)};
    return function_call<decltype(func_ptr), decltype(std::bind(func_ptr, std::forward<Args>(args)...)), Hasher>{
        std::bind(func_ptr, std::forward<Args>(args)...), func_ptr, parameter_hash};
}

} // namespace
#endif
