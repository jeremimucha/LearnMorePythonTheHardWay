#include <catch/catch.hpp>
#include <string>
#include <single_linked_list/single_linked_list.hpp>
using namespace std::string_literals;

TEST_CASE( "SingleLinkedList can be constructed" "[constructor]" )
{
    SECTION( "Default constructor" ){
        SingleLinkedList<std::string> sll0;
        auto sll1 = SingleLinkedList<std::string>();

        REQUIRE(sll0.empty());
        REQUIRE(sll1.empty());
    }

    SECTION( "Size constructor" ){
        const int big_count{1000000};
        auto sll = SingleLinkedList<std::string>(big_count);

        REQUIRE(!sll.empty());
        REQUIRE(sll.front().empty());
        REQUIRE(sll.size() == big_count);
    }

    SECTION( "Size constructor with value" ){
        const int small_count{42};
        const auto test_value = "test string"s;
        auto small = SingleLinkedList<std::string>(small_count, test_value);

        REQUIRE(!small.empty());
        REQUIRE(small.front() == test_value);
        REQUIRE(small.size() == small_count);
    }

    SECTION( "initializer_list constructor" ){
        auto ilist = {"one"s, "two"s, "three"s, "four"s, "five"s, "42"s};
        auto sll = SingleLinkedList<std::string>(ilist);

        REQUIRE(!sll.empty());
        REQUIRE(sll.front() == *ilist.begin());
        REQUIRE(sll.size() == ilist.size());
    }
}
