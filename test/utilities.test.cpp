#include "catch.hpp"

#include <vn/utilities.h>

#include <type_traits>
#include <tuple>

TEST_CASE("utilities", "[utilities]")
{
    // types we use in the test
    using std::is_same;
    using std::tuple;

    // types we test
    using vn::void_type;
    using vn::identity;
    using vn::size_of_pack;
    using vn::index_of;
    using vn::remove_rvalue_reference;
    using vn::contains;

    SECTION("basic tests")
    {

        CHECK((is_same<void_type<int>, void>{}));
        CHECK((is_same<identity<int>, int>{}));
        CHECK((size_of_pack<tuple<int, char>>{} == 2));
        CHECK((index_of<int, char, double, float>{} == -1));
        CHECK((index_of<int, char, int, float>{} == 1));
        CHECK((index_of<int, tuple<char, int, float>>{} == 1));
        CHECK((index_of<tuple<int>, tuple<int>, char, float>{} == 0));
        CHECK((is_same<int, remove_rvalue_reference<int&&>>{}));
        CHECK_FALSE((is_same<int, remove_rvalue_reference<int&>>{}));
        CHECK((contains<tuple<int, char, float>, float>{}));
        CHECK_FALSE((contains<tuple<int, char, float>, double>{}));
    }
}