#include "catch.hpp"

#include <vn/function_call.h>
#include <functional>

struct foo {
    int ret_zero(char, float)
    {
        return 0;
    }
    int ret_zero_(char, float)
    {
        return 0;
    }
};

int ret_zero(char, float)
{
    return 0;
}
int ret_zero_(char, float)
{
    return 0;
}

TEST_CASE("function_call", "[function_call]")
{
    // using
    using namespace std::placeholders;
    auto ret_zero_lambda = [](char, float) { return 0; };
    auto ret_zero_lambda_ = [](char, float) { return 0; };

    // testing
    using vn::make_function_call;

    SECTION("basic function_call tests")
    {
        auto fc = make_function_call(&ret_zero, _1, 0.2f);
        auto fc_memfn = make_function_call(&foo::ret_zero, _1, 'a', 0.2f);
        auto fc_lambda = make_function_call(ret_zero_lambda, _1, 0.2f);

        foo f;

        CHECK((fc('a') == 0));
        CHECK((fc_memfn(f) == 0));
        CHECK((fc_memfn(foo{}) == 0));
        CHECK((fc_lambda('a') == 0));

        auto fc_copy = fc;
        auto fc_memfncopy = fc_memfn;
        auto fc_lambdacopy = fc_lambda;

        CHECK((fc_copy('a') == 0));
        CHECK((fc_memfncopy(f) == 0));
        CHECK((fc_memfncopy(foo{}) == 0));
        CHECK((fc_lambdacopy('a') == 0));
    }

    SECTION("function_call comparison tests")
    {
        {
            auto fc_a = make_function_call(&ret_zero, _1, 0.1f);
            auto fc_b = make_function_call(&ret_zero, _1, 0.1f);
            auto fc_c = make_function_call(&ret_zero, _1, 0.2f);
            auto fc_d = make_function_call(&ret_zero_, _1, 0.1f);
            auto fc_e = make_function_call(&ret_zero_, _2, 0.1f);

            CHECK((fc_a == fc_b));
            CHECK_FALSE((fc_b == fc_c)); // different paramater value
            CHECK((fc_a.calls_same_function_as(fc_c)));
            CHECK_FALSE((fc_a.calls_same_function_as(fc_d))); // different function
            CHECK_FALSE((fc_d == fc_e));                      // different placeholder value
        }
        {
            auto fc_a = make_function_call(&foo::ret_zero, _1, 0.1f);
            auto fc_b = make_function_call(&foo::ret_zero, _1, 0.1f);
            auto fc_c = make_function_call(&foo::ret_zero, _1, 0.2f);
            auto fc_d = make_function_call(&foo::ret_zero_, _1, 0.1f);
            auto fc_e = make_function_call(&foo::ret_zero_, _2, 0.1f);

            CHECK((fc_a == fc_b));
            CHECK_FALSE((fc_b == fc_c)); // different paramater value
            CHECK((fc_a.calls_same_function_as(fc_c)));
            CHECK_FALSE((fc_a.calls_same_function_as(fc_d))); // different function
            CHECK_FALSE((fc_d == fc_e));                      // different placeholder value
        }
        {
            auto fc_a = make_function_call(ret_zero_lambda, _1, 0.1f);
            auto fc_b = make_function_call(ret_zero_lambda, _1, 0.1f);
            auto fc_c = make_function_call(ret_zero_lambda, _1, 0.2f);
            auto fc_d = make_function_call(ret_zero_lambda_, _1, 0.1f);
            auto fc_e = make_function_call(ret_zero_lambda_, _2, 0.1f);

            CHECK((fc_a == fc_b));
            CHECK_FALSE((fc_b == fc_c)); // different paramater value
            CHECK((fc_a.calls_same_function_as(fc_c)));
            CHECK_FALSE((fc_a.calls_same_function_as(fc_d))); // different function
            CHECK_FALSE((fc_d == fc_e));                      // different placeholder value
        }
        {
            auto fc_a = make_function_call(ret_zero_lambda, _1, 0.1f);
            auto fc_b = make_function_call(ret_zero_lambda, _1, 0.1f);
            auto fc_c = make_function_call(ret_zero_lambda, _1, 0.2f);
            auto fc_d = make_function_call(ret_zero_lambda_, _1, 0.1f);
            auto fc_e = make_function_call(ret_zero_lambda_, _2, 0.1f);

            CHECK((fc_a == fc_b));
            CHECK_FALSE((fc_b == fc_c)); // different paramater value
            CHECK((fc_a.calls_same_function_as(fc_c)));
            CHECK_FALSE((fc_a.calls_same_function_as(fc_d))); // different function
            CHECK_FALSE((fc_d == fc_e));                      // different placeholder value
        }
        {
            auto fc_a = make_function_call(&foo::ret_zero, _1, 0.1f);
            auto fc_b = make_function_call(&ret_zero, _1, 0.1f);
            auto fc_c = make_function_call(ret_zero_lambda, _1, 0.2f);

            CHECK_FALSE((fc_a == fc_b));                      // different types
            CHECK_FALSE((fc_b == fc_c));                      // different types
            CHECK_FALSE((fc_a.calls_same_function_as(fc_b))); // different types
            CHECK_FALSE((fc_b.calls_same_function_as(fc_c))); // different types
        }
    }
}