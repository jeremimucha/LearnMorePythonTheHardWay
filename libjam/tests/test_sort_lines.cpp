#include "catch/catch.hpp"
#include "jam/jam.hpp"
#include <vector>
#include <list>
#include <functional>

using std::vector;
using std::list;

TEST_CASE( "sort_lines", "[sort]" )
{
    auto test_vector = vector<int>{5,4,1,2,5,23,634,762,1,678,8};
    auto test_list = list<int>{5,4,1,2,5,23,634,762,1,678,8};

    auto predicate = std::less<int>();
    auto expected_vector = test_vector;
    std::sort(expected_vector.begin(), expected_vector.end(), predicate);
    auto expected_list = test_list;
    expected_list.sort(predicate);


    SECTION( "vectors are sorted" )
    {
        jam::sort_lines(test_vector, predicate);
        REQUIRE( test_vector == expected_vector );
    }

    SECTION( "lists are sorted" )
    {
        jam::sort_lines(test_list, predicate);
        REQUIRE( test_list == expected_list );
    }

    
}
