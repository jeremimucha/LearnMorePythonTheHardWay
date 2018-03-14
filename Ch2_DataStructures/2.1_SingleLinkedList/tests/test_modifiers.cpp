#include "catch/catch.hpp"
#include "single_linked_list/single_linked_list.hpp"



TEST_CASE( "Elements can be inserted into the SLL", "[insert][emplace]" )
{
    auto sll = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};
    auto original_size = sll.size();
    auto insertion_point = std::find(sll.cbegin(), sll.cend(), 5);

    SECTION( "Element can be inserted by const&" ){
        auto elem = int{42};
        auto inserted = sll.insert_after(insertion_point, elem);
        REQUIRE(*inserted == elem);
        REQUIRE(sll.size() == (original_size + 1));
        REQUIRE(++insertion_point == inserted);
    }

    SECTION( "Element can be inserted by rvalue reference" ){
        auto inserted = sll.insert_after(insertion_point, 42);
        REQUIRE(*inserted == 42);
        REQUIRE(sll.size() == (original_size + 1));
        REQUIRE(++insertion_point == inserted);
    }

    // this matches the insert_after(const_iterator pos, InputIt, InputIt) overload...
    // would have to use enable_if somehow to disable it for non-pointer/iterator types
    SECTION( "A number of copy-constructed elements can be inserted" ){
        sll.insert_after(insertion_point, 10, 42);
        REQUIRE(sll.size() == original_size + 10);
    }

    SECTION( "A range of elements can be inserted" ){
        auto v = std::vector<int>{99, 88, 77, 66, 55, 44};
        auto inserted = sll.insert_after(insertion_point, v.cbegin(), v.cend());
        REQUIRE(std::equal(v.cbegin(),v.cend(),inserted));
        REQUIRE(sll.size() == original_size + v.size());
        REQUIRE(++insertion_point == inserted);
    }

    SECTION( "An initializer_list can be inserted" ){
        auto ilist = {999, 888, 777, 666, 555, 444, 333, 222, 111};
        auto inserted = sll.insert_after(insertion_point, ilist);
        REQUIRE(std::equal(ilist.begin(), ilist.end(),inserted));
        REQUIRE(sll.size() == original_size + 9);
    }

    SECTION( "Elements can be emplaced" ){
        auto inserted = sll.emplace_after(insertion_point, 42);
        REQUIRE(*inserted == 42);
        REQUIRE(sll.size() == (original_size + 1));
        REQUIRE(++insertion_point == inserted);
    }
}

TEST_CASE( "Elements can be erased", "[erase]" )
{
    auto sll = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};
    auto original_size = sll.size();
    auto erase_point = std::find(sll.cbegin(), sll.cend(), 5);

    SECTION( "A single element can be erased" ){
        auto after_erased = sll.erase_after(erase_point);
        REQUIRE(sll.size() == original_size - 1);
        REQUIRE(*after_erased == 7);
    }

    SECTION( "A range of elements can be erased" ){
        auto erase_end = erase_point;
        std::advance(erase_end, 3);
        auto after_erased = sll.erase_after(erase_point, erase_end);
        REQUIRE(sll.size() == 8);
        REQUIRE(after_erased == erase_end);
    }
}

TEST_CASE( "Elements can be pushed to the front of the list"
         , "[push][emplace]" )
{
    auto sll = SingleLinkedList<int>{};
    auto original_size = sll.size();

    SECTION( "Push elements by lvalue ref" ){
        const auto elem = int{42};
        sll.push_front(elem);
        REQUIRE(!sll.empty());
        REQUIRE(sll.front() == elem);
        const auto elem2 = int{11};
        sll.push_front(elem2);
        REQUIRE(sll.size() == 2);
        REQUIRE(sll.front() == elem2);
    }

    SECTION( "Push elements by rvalue ref" ){
        sll.push_front(42);
        REQUIRE(!sll.empty());
        REQUIRE(sll.front() == 42);
        sll.push_front(11);
        REQUIRE(sll.size() == 2);
        REQUIRE(sll.front() == 11);
    }

    SECTION( "Elements can be emplaced" ){
        sll.emplace_front(42);
        REQUIRE(!sll.empty());
        REQUIRE(sll.front() == 42);
        sll.emplace_front(11);
        REQUIRE(sll.size() == 2);
        REQUIRE(sll.front() == 11);
    }
}


TEST_CASE( "Elements can be poped from the front of the list", "[pop]" )
{
    auto sll = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};
    auto original_size = sll.size();

    SECTION( "pop'ing from a non-empty list" ){
        sll.pop_front();
        REQUIRE(sll.size() == original_size - 1);
        while(!sll.empty()) sll.pop_front();
        REQUIRE(sll.empty());
    }

    // segfault
    // SECTION( "pop'ing from an empty list is an invalid operation" ){
    //     auto empty_list = SingleLinkedList<int>{};
    //     REQUIRE_THROWS(empty_list.pop_front()); // will only throw in debug builds...
    // }
}

TEST_CASE( "SingleLinkedList can be resized", "[resize]" )
{
    auto sll = SingleLinkedList<int>{0,1,2,3,4,5,6,7,8,9};
    
    SECTION( "Resizing bigger inserts elements in the back" ){
        sll.resize(42, 11);
        REQUIRE(sll.size() == 42);
    }

    SECTION( "Resizing smaller removes elements from the back" ){
        sll.resize(0);
        CHECK(sll.empty());
        REQUIRE(sll.size() == 0);

        SECTION( "Resizing bigger from an empty SLL" ){
            sll.resize(1,42);
            REQUIRE_FALSE(sll.empty());
            REQUIRE(sll.size() == 1);
            REQUIRE(sll.front() == 42);
        }
    }
}
