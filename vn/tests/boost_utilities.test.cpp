#include "catch.hpp"

#include <vn/include/boost_utilities.h>

#include <cstddef>
#include <type_traits>
#include <tuple>

struct foo { friend std::size_t hash_value(foo const&) { return 55; } };

TEST_CASE("boost_utilities","[boost_utilities]") {                    
    // types we use in the test
    using std::is_same;
    using std::tuple;

    using v_one = boost::variant<int,char,foo>;
    using v_two = boost::variant<boost::blank,int,double,float>;
    using v_merged = boost::variant<int,char,foo,boost::blank,double,float>; 
       
    // types we test
    using vn::extended_variant;
    using vn::variant_over;
    using vn::boost_hasher;

    SECTION("basic tests") {                
        CHECK((    is_same<extended_variant<v_one,v_two>, v_merged>{}     ));        
        CHECK((    is_same<variant_over<v_one,v_two>, v_merged>{}         ));        
        CHECK((    is_same<variant_over<int,char,foo,v_two>, v_merged>{}  ));
        CHECK((    is_same<variant_over<int,char,foo>, v_one>{}           ));            
    }

    SECTION("vn::boost_hasher tests") {
        boost_hasher b;

        CHECK((    b == boost_hasher{}           ));             
        
        b(1)(2)(3);

        CHECK((    b == boost_hasher{}(1,2,3)    ));
        CHECK((    boost_hasher{}(foo{},'a',3.0) == boost_hasher{}(foo{})('a')(3.0)    ));
  CHECK_FALSE((    boost_hasher{}(foo{},1,1) == boost_hasher{}(foo{})(1)(2)    ));            
    }
}