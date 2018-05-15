#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "BinarySearch/BinarySearch.h"


namespace
{

class BinarySearchTest : public ::testing::Test { };


TEST_F( BinarySearchTest, EmptyContainer )
{
    const auto container = std::vector<int>{};
    auto it = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_EQ(it, container.cend());
}

TEST_F( BinarySearchTest, SingleElement )
{
    const auto container = std::vector<int>{1};
    auto found_it = binarySearch(container.cbegin(), container.cend(), 1);
    ASSERT_NE(found_it, container.cend());
    ASSERT_EQ(*found_it, 1);

    auto not_found_min = binarySearch(container.cbegin(), container.cend(), -42);
    ASSERT_EQ(not_found_min, container.cend());
    auto not_found = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_EQ(not_found, container.cend());
}

TEST_F( BinarySearchTest, TwoElements )
{
    const auto container = std::vector<int>{1,4};
    auto found1 = binarySearch(container.cbegin(), container.cend(), 1);
    ASSERT_NE(found1, container.cend());
    ASSERT_EQ(*found1, 1);
    auto found4 = binarySearch(container.cbegin(), container.cend(), 4);
    ASSERT_NE(found4, container.cend());
    ASSERT_EQ(*found4, 4);

    auto not_found_min = binarySearch(container.cbegin(), container.cend(), -42);
    ASSERT_EQ(not_found_min, container.cend());
    auto not_found = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_EQ(not_found, container.cend());
}

TEST_F( BinarySearchTest, ThreeElements )
{
    const auto container = std::vector<int>{1,4,7};
    auto found1 = binarySearch(container.cbegin(), container.cend(), 1);
    ASSERT_NE(found1, container.cend());
    ASSERT_EQ(*found1, 1);
    auto found4 = binarySearch(container.cbegin(), container.cend(), 4);
    ASSERT_NE(found4, container.cend());
    ASSERT_EQ(*found4, 4);
    auto found7 = binarySearch(container.cbegin(), container.cend(), 7);
    ASSERT_NE(found7, container.cend());
    ASSERT_EQ(*found7, 7);

    auto not_found_min = binarySearch(container.cbegin(), container.cend(), -42);
    ASSERT_EQ(not_found_min, container.cend());
    auto not_found = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_EQ(not_found, container.cend());
}

TEST_F( BinarySearchTest, FourElements )
{
    const auto container = std::vector<int>{1,4,7,11};
    auto found1 = binarySearch(container.cbegin(), container.cend(), 1);
    ASSERT_NE(found1, container.cend());
    ASSERT_EQ(*found1, 1);
    auto found4 = binarySearch(container.cbegin(), container.cend(), 4);
    ASSERT_NE(found4, container.cend());
    ASSERT_EQ(*found4, 4);
    auto found7 = binarySearch(container.cbegin(), container.cend(), 7);
    ASSERT_NE(found7, container.cend());
    ASSERT_EQ(*found7, 7);
    auto found11 = binarySearch(container.cbegin(), container.cend(), 11);
    ASSERT_NE(found11, container.cend());
    ASSERT_EQ(*found11, 11);

    auto not_found_min = binarySearch(container.cbegin(), container.cend(), -42);
    ASSERT_EQ(not_found_min, container.cend());
    auto not_found = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_EQ(not_found, container.cend());
}

TEST_F( BinarySearchTest, 1kElements )
{
    auto container = std::vector<int>{};
    container.reserve(1000);
    for(int i=0; i<1000; ++i) container.push_back(i);

    auto found_min = binarySearch(container.cbegin(), container.cend(), container.front());
    ASSERT_NE(found_min, container.end());
    ASSERT_EQ(*found_min, container.front());

    auto found_max = binarySearch(container.cbegin(), container.cend(), container.back());
    ASSERT_NE(found_max, container.cend());
    ASSERT_EQ(*found_max, container.back());

    auto found = binarySearch(container.cbegin(), container.cend(), 42);
    ASSERT_NE(found, container.cend());
    ASSERT_EQ(*found, 42);

    auto not_found_min = binarySearch(container.cbegin(), container.cend(), -42);
    ASSERT_EQ(not_found_min, container.cend());

    auto not_found_max = binarySearch(container.cbegin(), container.cend(), 9999);
    ASSERT_EQ(not_found_max, container.cend());

}

} // namespace
