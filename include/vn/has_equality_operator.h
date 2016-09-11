/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef BAASFIWARRRAGG_VN_HAS_EQUALITY_OPERATOR_
#define BAASFIWARRRAGG_VN_HAS_EQUALITY_OPERATOR_

#include <cstddef>
#include <type_traits>
#include <utility>

namespace vn {

template <typename T>
struct has_equality_operator {

    using no = char[1];
    using yes = char[2];

    template <typename Q>
    static yes& check(decltype(std::declval<Q&>() == std::declval<Q&>()) *);

    template <typename>
    static no& check(...);

    static constexpr bool value = sizeof(check<T>(0)) == sizeof(yes);
    constexpr operator bool() const
    {
        return value;
    }
};

} // namespace
#endif
