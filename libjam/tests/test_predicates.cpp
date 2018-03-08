#include "catch/catch.hpp"
#include "jam/jam.hpp"
#include <functional>
#include <algorithm>
#include <vector>
#include <iterator>

#include <iostream>

using std::vector;

TEST_CASE( "creating basic PredicateWrapper", "[PredicateWrapper]" )
{
    auto predicate = [](int i){return i%2 == 0;};
    auto action = [](int i){return i*2;};
    auto test_input = vector<int>{53,4,2,1,6,34,2,76,234,7452,12,3,35,623};
    
    SECTION( "wrap_predicate single transform" ){
        auto wpred = jam::wrap_predicate(predicate, action);
        REQUIRE( true == std::all_of(test_input.cbegin(),
                            test_input.cend(), wpred));
    }

    SECTION( "wrap_predicate - multiple transforms" ){
        auto action2 = [](int i){ return i+1; };
        auto wpred = jam::wrap_predicate(predicate, action2, action );
        REQUIRE( true == std::none_of(test_input.cbegin(),
                            test_input.cend(), wpred) );
    }
}


TEST_CASE( "transform_iterator" )
{
    auto test_input = std::vector<int>{0,1,2,3,4,5,6,7,8,9};
    auto transform = [](int i){ std::cout << "ufunc " << i << std::endl; return ++i; };
    auto expected = test_input;
    std::transform(expected.begin(),expected.end(),expected.begin(), transform);

    SECTION( "bla" ){
        auto result = std::vector<int>{};
        std::copy(jam::make_transform_iterator(test_input.cbegin(), transform)
                 ,jam::make_transform_iterator(test_input.cend(), transform)
                 ,std::back_inserter(result)
                 );
        REQUIRE( result == expected );
    }
}