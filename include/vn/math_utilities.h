/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef BNOWOAOWFWZSLFFFG_VN_MATH_UTILITIES_H_
#define BNOWOAOWFWZSLFFFG_VN_MATH_UTILITIES_H_

#include <cmath>

namespace vn {
namespace math {

    // template <typename T> const T pi = std::acos(-1.0);
    // template <typename T> const T deg_to_rad = std::acos(-1.0) / 180.0;
    // template <typename T> const T rad_to_deg = 180.0 / std::acos(-1.0);

    template <typename T = float>
    constexpr T pi() noexcept
    {
        return static_cast<T>(std::acos(-1.0));
    }

    template <typename T = float>
    constexpr T deg_to_rad(T deg) noexcept
    {
        return static_cast<T>(deg * (pi<double>() / 180.0));
    }

    template <typename T = float>
    constexpr T rad_to_deg(T rad) noexcept
    {
        return static_cast<T>(rad * (180.0 / pi<double>()));
    }

    float rand_zero_one();
    float rand_neg_one_one();
}
} // namespace
#endif
