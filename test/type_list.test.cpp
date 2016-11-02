#include "catch.hpp"

#include <vn/type_list.h>

#include <type_traits>
#include <tuple>

template <typename T>
using is_same_as_char = std::is_same<char, T>;

TEST_CASE("type_list", "[type_list]")
{

    // types we use
    using std::is_same;
    using std::tuple;

    // types we test
    using vn::type_list;

    SECTION("various type_list operations")
    {
        using t = type_list<int, char, float>;
        REQUIRE((is_same<t, type_list<int, char, float>>{}));

        // ::filter
        CHECK((is_same<type_list<int, char, float>::filter<int, char>, type_list<float>>{}));
        CHECK((is_same<type_list<int, char, float>::filter<double>, type_list<int, char, float>>{}));
        CHECK((is_same<t::filter<>, t>{}));
        CHECK((is_same<type_list<>::filter<>, type_list<>>{}));
        CHECK((is_same<type_list<>::filter<double>, type_list<>>{}));
        CHECK((is_same<type_list<double>::filter<double>, type_list<>>{}));
        // ::join
        CHECK((is_same<t::join<>, type_list<int, char, float>>{}));
        CHECK((is_same<t::join<double>, type_list<int, char, float, double>>{}));
        CHECK((is_same<t::join<type_list<double>>, type_list<int, char, float, type_list<double>>>{}));
        // ::unique
        CHECK((is_same<t::unique, type_list<int, char, float>>{}));
        CHECK((is_same<type_list<>::unique, type_list<>>{}));
        // ::merge
        CHECK((is_same<t::merge<>, type_list<int, char, float>>{}));
        CHECK((is_same<t::merge<double>, type_list<int, char, float, double>>{}));
        CHECK((is_same<t::merge<type_list<double>>, type_list<int, char, float, double>>{}));
        CHECK((is_same<t::merge<char, type_list<int, double>, float>, type_list<int, char, float, char, int, double, float>>{}));
        // ::discharge
        CHECK((is_same<t::discharge<tuple>, std::tuple<int, char, float>>{}));
        // ::each_satisfies
        CHECK((t::each_satisfies<std::is_arithmetic>{}));     // all are arithmetic
        CHECK_FALSE((t::each_satisfies<std::is_integral>{})); // float is not integral
        // ::any_satisfies
        CHECK((t::any_satisfies<std::is_arithmetic>{}));    // all are arithmetic, so any is..
        CHECK_FALSE((t::any_satisfies<std::is_pointer>{})); // none are pointers
        // ::one_satisfies
        CHECK((t::one_satisfies<is_same_as_char>{}));        // only one is char
        CHECK_FALSE((t::one_satisfies<std::is_integral>{})); // more than one is integral
    }
}