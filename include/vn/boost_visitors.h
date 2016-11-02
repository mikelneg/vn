/*-------------------------------------------------------------

Copyright (c) 2016 Mikel Negugogor (http://github.com/mikelneg)
MIT license. See LICENSE.txt in project root for details.

---------------------------------------------------------------*/

#ifndef YOWBOZLSDKJFE_VN_BOOST_VISITORS_H_
#define YOWBOZLSDKJFE_VN_BOOST_VISITORS_H_

#include <cstddef>
//#include <type_traits>
#include <boost/variant.hpp>
#include <vn/has_equality_operator.h>

namespace vn {
namespace visitors {

    template <typename T>
    struct has_variant_type;
    struct same_type;
    struct weak_equality; // if same type and operator== comparable, returns operator== result
                          // if same type but not operator== comparable, returns true
                          // if different types, returns false

    // struct strict_equality; // as above, but if same type but not operator== comparable, returns false

    template <typename T>
    struct has_variant_type : boost::static_visitor<bool> {
        template <typename Q>
        constexpr bool operator()(Q const&) const
        {
            return false;
        }
        constexpr bool operator()(T const&) const
        {
            return true;
        }
    };

    template <typename T, typename R>
    bool has_type(R const& r)
    {
        return has_variant_type<T>{}(r);
    }

    struct same_type : boost::static_visitor<bool> {
        template <typename T, typename Q>
        constexpr bool operator()(T const&, Q const&) const
        {
            return false;
        }

        template <typename T>
        constexpr bool operator()(T const&, T const&) const
        {
            return true;
        }
    };

    struct weak_equality : boost::static_visitor<bool> {
        template <typename T>
        constexpr std::enable_if_t<vn::has_equality_operator<T>::value, bool> operator()(T const& lhs, T const& rhs) const
        {
            return lhs == rhs;
        }

        template <typename T>
        constexpr std::enable_if_t<!vn::has_equality_operator<T>::value, bool> operator()(T const&, T const&) const
        {
            return true;
        }

        template <typename T, typename Q>
        constexpr bool operator()(T const&, Q const&) const
        {
            return false;
        }
    };

    struct strong_equality : boost::static_visitor<bool> {
        template <typename T>
        constexpr std::enable_if_t<vn::has_equality_operator<T>::value, bool> operator()(T const& lhs, T const& rhs) const
        {
            return lhs == rhs;
        }

        template <typename T>
        constexpr std::enable_if_t<!vn::has_equality_operator<T>::value, bool> operator()(T const&, T const&) const
        {
            return false;
        }

        template <typename T, typename Q>
        constexpr bool operator()(T const&, Q const&) const
        {
            return false;
        }
    };
}
} // namespace
#endif
