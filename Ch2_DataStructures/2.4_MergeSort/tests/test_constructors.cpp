#include "DoubleLinkedList/DoubleLinkedList.hpp"
#include <algorithm>
#include <vector>
#include "gtest/gtest.h"

namespace
{

using detail::ensure_invariant;

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

class AssignmentTest : public ::testing::Test
{
protected:
    DoubleLinkedList<int> f_empty{};
    DoubleLinkedList<int> f_dllA{0,1,2,3,4,5,6,7,8,9};
    DoubleLinkedList<int> f_dllB = DoubleLinkedList<int>(42,42);
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

TEST_F( AssignmentTest, Copy )
{
    f_empty = f_dllA;
    ASSERT_TRUE(ensure_invariant(f_empty));
    ASSERT_TRUE(ensure_invariant(f_dllA));
    ASSERT_EQ(f_empty.size(),f_dllA.size());
    ASSERT_TRUE(std::equal(f_empty.cbegin(), f_empty.cend(),f_dllA.cbegin()));
    f_dllA = f_dllB;
    ASSERT_TRUE(ensure_invariant(f_empty));
    ASSERT_TRUE(ensure_invariant(f_dllB));
    ASSERT_TRUE(ensure_invariant(f_dllA));
    ASSERT_EQ(f_dllB.size(),f_dllA.size());
    ASSERT_TRUE(std::equal(f_dllB.cbegin(), f_dllB.cend(),f_dllA.cbegin()));
}

TEST_F( AssignmentTest, Move )
{
    const auto dllA_cpy = f_dllA;
    const auto dllB_cpy = f_dllB;
    f_dllA = std::move(f_dllB);
    ASSERT_TRUE(ensure_invariant(f_dllA));
    ASSERT_TRUE(ensure_invariant(f_dllB));
    ASSERT_EQ(f_dllA.size(),dllB_cpy.size());
    ASSERT_TRUE(std::equal(f_dllA.cbegin(),f_dllA.cend(),dllB_cpy.cbegin()));
}

TEST_F( AssignmentTest, InitializerList )
{
    auto ilist = {111,222,333,444,555,666};
    f_dllA = ilist;
    ASSERT_TRUE(ensure_invariant(f_dllA));
    ASSERT_EQ(f_dllA.size(), 6);
    ASSERT_TRUE(std::equal(f_dllA.cbegin(),f_dllA.cend(),ilist.begin()));
}

} // namespace
