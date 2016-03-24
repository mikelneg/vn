#include "catch.hpp"

#include <vn/include/boost_utilities.h>
#include <vn/include/type_list.h>

#include <cstddef>
#include <type_traits>
#include <tuple>

struct foo { friend std::size_t hash_value(foo const&) { return 55; } };

TEST_CASE("boost_utilities","[boost_utilities]") {                    
    // types we use in the test
    using std::is_same;
    using std::tuple;
    using vn::type_list;

    using v_one = boost::variant<int,char,foo>;
    using v_two = boost::variant<boost::blank,int,double,float>;
    using v_merged = boost::variant<int,char,foo,boost::blank,double,float>; 
       
    // types we test
    using vn::extended_variant;
    using vn::variant_over;
    using vn::boost_hasher;
    using vn::mpl_sequence_to_list;
    using vn::zip_mpl_sequences;
    using vn::variant_contains;

    // functions we test
    using vn::make_variant_vector;
    using vn::make_variant_vector_with_allocator;
    using vn::make_lambda_visitor;

    SECTION("basic tests") 
    {                
        CHECK((    is_same<extended_variant<v_one,v_two>, v_merged>{}     ));        
        CHECK((    is_same<variant_over<v_one,v_two>, v_merged>{}         ));        
        CHECK((    is_same<variant_over<int,char,foo,v_two>, v_merged>{}  ));
        CHECK((    is_same<variant_over<int,char,foo>, v_one>{}           ));
        CHECK((    variant_contains<v_one,int>{}                          ));
  CHECK_FALSE((    variant_contains<v_one,float>{}                        ));
    }

    SECTION("vn::boost_hasher tests") 
    {
        boost_hasher b;

        CHECK((    b == boost_hasher{}    ));   // default-initializes
        
        b(1)(2)(3);

        CHECK((    b == boost_hasher{}(1,2,3)    )); // lvalues and rvalues compare equal

        CHECK((    boost_hasher{}(foo{},'a',3.0) == boost_hasher{}(foo{})('a')(3.0)    ));
        
        CHECK_FALSE((    boost_hasher{}(foo{},1,1) == boost_hasher{}(foo{})(1)(2)    ));           
    }

    SECTION("vn::make_variant_vector tests") 
    {
        {
            auto vv = make_variant_vector<int,char,float>();
            CHECK((    vv.size() == 0    ));    
            CHECK((    is_same<decltype(vv)::value_type, boost::variant<int,char,float>>{}    ));
        }
        {
            auto vv = make_variant_vector(0,'a',0.1f);
            CHECK((    vv.size() == 3    ));
            CHECK((    is_same<decltype(vv)::value_type, boost::variant<int,char,float>>{}    ));
        }
    }

    SECTION("vn::make_lambda_visitor tests") 
    {        
        auto vv = make_variant_vector(0,'a',foo{});
        {
            int i{};
            auto lv = make_lambda_visitor([&](int){ ++i; },
                                          [&](foo&){ ++i; },
                                          [&](char){ ++i; });                            
            for (auto&& e : vv) { apply_visitor(lv,e); }    
            CHECK((    i == 3   ));
        }
        {
            int i{};
            auto lv = make_lambda_visitor([&](auto&){ ++i; },  
                                          [&](int){ i += 2; });

            for (auto&& e : vv) { apply_visitor(lv,e); }    
            CHECK((    i == 4   ));
        }
        {
            int i{};
            auto lv = make_lambda_visitor([&](int){ i += 2; },
                                          [&](auto&){ ++i; });

            for (auto&& e : vv) { apply_visitor(lv,e); }    
            CHECK((    i == 4   ));
        }        
        {
            int i{};
            auto lv = make_lambda_visitor([&](int){ ++i; },
                                          [&](char){ ++i; },
                                          [&](foo&){ ++i; });                            
            
            auto deferred_visitor = apply_visitor(lv);

            for (auto&& e : vv) { deferred_visitor(e); }    
            CHECK((    i == 3   ));            
        }
    }

    SECTION("tools for boost::mpl tests")
    {
        using v_one = boost::variant<int,char,foo>;
        using v_two = boost::variant<boost::blank,int,double,float>;
        using v_merged = boost::variant<int,char,foo,boost::blank,double,float>;         

        using tlist_one = type_list<int,char,foo,boost::blank,int,double,float>; // not unique'd
        using tlist_two = tlist_one::unique; // unique'd

        using zipped_ = zip_mpl_sequences<v_one,v_two>::unique::discharge<boost::variant>;

        CHECK((    is_same<mpl_sequence_to_list<v_one>, type_list<int,char,foo>>{}    ));        
        CHECK((    is_same<zip_mpl_sequences<v_one,v_two>, tlist_one>{}    ));             
  CHECK_FALSE((    is_same<zip_mpl_sequences<v_one,v_two>, tlist_two>{}    ));        
        CHECK((    is_same<zipped_, v_merged>{}    ));
    }
}