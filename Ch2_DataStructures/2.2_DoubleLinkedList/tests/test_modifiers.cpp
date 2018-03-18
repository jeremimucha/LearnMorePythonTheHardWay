#include "gtest/gtest.h"
#include "DoubleLinkedList/DoubleLinkedList.hpp"
#include <algorithm>
#include <iterator>


namespace
{

class InsertTest : public ::testing::Test
{
    void SetUp() override
    {
        ensure_invariant(f_dll);
    }
protected:
    DoubleLinkedList<int> f_dll{0,1,2,3,4,5,6,7,8,9};
};


TEST_F( InsertTest, ConstLvalue )
{
    const int value = 42;
    f_dll.insert(f_dll.cbegin(), value);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.front(), value);
    
    f_dll.insert(f_dll.cend(), value);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.back(), value);
    
    auto it = f_dll.begin();
    std::advance(it, 5);
    auto inserted = f_dll.insert(it, value);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(--it, inserted);
    ASSERT_EQ(*inserted, value);
}

TEST_F( InsertTest, Rvalue )
{
    f_dll.insert(f_dll.cbegin(), 42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.front(), 42);
    
    f_dll.insert(f_dll.cend(), 42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.back(), 42);
    
    auto it = f_dll.begin();
    std::advance(it, 5);
    auto inserted = f_dll.insert(it, 42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(--it, inserted);
    ASSERT_EQ(*inserted, 42);
}

TEST_F( InsertTest, Range )
{
    const auto original_size = f_dll.size();
    const auto range = std::vector<int>{99,88,77,66,55,44,33};
    auto it1 = f_dll.insert(f_dll.cbegin(), range.cbegin(), range.cend());
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_TRUE(std::equal(range.cbegin(),range.cend(),it1));
    
    auto it2 = f_dll.insert(f_dll.cend(), range.cbegin(), range.cend());
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_TRUE(std::equal(range.cbegin(),range.cend(),it2));
    
    auto ipoint = f_dll.begin();
    std::advance(ipoint, 5);
    auto it3 = f_dll.insert(ipoint, range.cbegin(), range.cend());
    ASSERT_TRUE(ensure_invariant(f_dll));
    std::advance(ipoint, -range.size());
    ASSERT_EQ(ipoint, it3);
    ASSERT_TRUE(std::equal(range.cbegin(),range.cend(),it3));

    ASSERT_EQ(f_dll.size(), original_size + range.size()*3);
}

} // namespace
