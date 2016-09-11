/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef UTUOWBBLXKJFLWWWF_VN_COMPOSITE_FUNCTION_H_
#define UTUOWBBLXKJFLWWWF_VN_COMPOSITE_FUNCTION_H_

#include <type_traits>
#include <utility>

namespace vn {

template <typename, typename...>
struct composite_function;

template <typename T>
struct composite_function<T> : T {

    template <typename Q>
    constexpr composite_function(Q&& q) noexcept(noexcept(T(std::forward<Q>(q))))
        : T(std::forward<Q>(q))
    {
    }

    using T::operator();
};

template <typename H, typename T, typename... Ts>
struct composite_function<H, T, Ts...> : H, composite_function<T, Ts...> {

    template <typename Q, typename... Qs>
    constexpr composite_function(Q&& q, Qs&&... qs) noexcept(noexcept(H(std::forward<Q>(q))) && noexcept(composite_function<T, Ts...>(std::forward<Qs>(qs)...)))
        : H(std::forward<Q>(q)), composite_function<T, Ts...>(std::forward<Qs>(qs)...)
    {
    }

    using H::operator();
    using composite_function<T, Ts...>::operator();
};

template <typename F, typename... Fs>
constexpr auto make_composite_function(F&& f, Fs&&... fs)
{
    return composite_function<std::remove_reference_t<F>, std::remove_reference_t<Fs>...>{std::forward<F>(f), std::forward<Fs>(fs)...};
}

} // namespace
#endif
