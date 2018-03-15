#include "DoubleLinkedList/DoubleLinkedList.hpp"
#include <algorithm>
#include <vector>
#include "gtest/gtest.h"

namespace
{

class ConstructorTest : public ::testing::Test
{
protected:
    int f_count{11};
    int f_value{42};
    std::initializer_list<int> f_ilist{0,1,2,3,4,5,6,7,8,9};
};

class CopyControlTest : public ::testing::Test
{
protected:
    DoubleLinkedList<int> f_refobj{0,1,2,3,4,5,6,7,8,9};
};


TEST_F( ConstructorTest, Default )
{
    const DoubleLinkedList<int> dll{};
    ASSERT_TRUE(ensure_invariant(dll));
    ASSERT_TRUE(dll.empty());
    ASSERT_EQ(dll.size(), 0);
}

TEST_F( ConstructorTest, Count )
{
    const DoubleLinkedList<int> dll(f_count);
    ASSERT_TRUE(ensure_invariant(dll));
    ASSERT_FALSE(dll.empty());
    ASSERT_EQ(dll.size(), f_count);
    ASSERT_EQ(dll.front(), 0);
}

TEST_F( ConstructorTest, CountValue )
{
    const DoubleLinkedList<int> dll(f_count, f_value);
    ASSERT_TRUE(ensure_invariant(dll));
    ASSERT_FALSE(dll.empty());
    ASSERT_EQ(dll.size(), f_count);
    ASSERT_EQ(dll.front(), f_value);
    ASSERT_EQ(dll.back(), f_value);
}

TEST_F( ConstructorTest, InitializerList )
{
    const DoubleLinkedList<int> dll(f_ilist);
    ASSERT_TRUE(ensure_invariant(dll));
    ASSERT_FALSE(dll.empty());
    ASSERT_EQ(dll.size(), 10);
    ASSERT_TRUE(std::equal(dll.cbegin(), dll.cend(), f_ilist.begin()));
}

TEST_F( CopyControlTest, Copy )
{
    const DoubleLinkedList<int> cpy(f_refobj);
    ASSERT_TRUE(ensure_invariant(cpy));
    ASSERT_TRUE(ensure_invariant(f_refobj));
    ASSERT_EQ(cpy.size(), f_refobj.size());
    ASSERT_TRUE(std::equal(cpy.cbegin(), cpy.cend(), f_refobj.cbegin()));
}

TEST_F( CopyControlTest, Move )
{
    const auto ref_size = f_refobj.size();
    const std::vector<int> ref_range(f_refobj.cbegin(), f_refobj.cend());
    const DoubleLinkedList<int> mv(std::move(f_refobj));
    ASSERT_TRUE(ensure_invariant(mv));
    ASSERT_TRUE(ensure_invariant(f_refobj));
    ASSERT_EQ(mv.size(), ref_size);
    ASSERT_TRUE(std::equal(mv.cbegin(), mv.cend(), ref_range.cbegin()));
}

} // namespace
