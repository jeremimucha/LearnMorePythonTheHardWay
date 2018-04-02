#include "gtest/gtest.h"
#include "DoubleLinkedList/DoubleLinkedList.hpp"
#include <algorithm>
#include <iterator>
#include <chrono>
#include <random>
#include <iostream>


namespace
{

using detail::ensure_invariant;
std::uniform_int_distribution<> ud{0,100};
std::default_random_engine re{
    static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())};


class ModifiersTest : public ::testing::Test
{
    void SetUp() override
    {
        ensure_invariant(f_dll);
    }
protected:
    using size_type = typename DoubleLinkedList<int>::size_type;
    DoubleLinkedList<int> f_dll{0,1,2,3,4,5,6,7,8,9};
    const size_type f_original_size{f_dll.size()};
};

class SortTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for(auto i=0; i<20; ++i){
            f_random_list.push_back(ud(re));
        }
        ASSERT_TRUE(ensure_invariant(f_empty_list));
        ASSERT_TRUE(ensure_invariant(f_single_elem_list));
        ASSERT_TRUE(ensure_invariant(f_random_list));
        ASSERT_TRUE(f_empty_list.empty());
        ASSERT_FALSE(f_single_elem_list.empty());
        ASSERT_FALSE(f_random_list.empty());
    }

    DoubleLinkedList<int> f_empty_list{};
    DoubleLinkedList<int> f_single_elem_list{42};
    DoubleLinkedList<int> f_two_elem_list{42, 11};
    DoubleLinkedList<int> f_random_list{};
};

std::ostream& operator<<(std::ostream& os, const DoubleLinkedList<int>& list)
{
    for( const auto i : list )
        os << i << " ";
    return os;
}

TEST_F( ModifiersTest, ConstLvalue )
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

TEST_F( ModifiersTest, Rvalue )
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

TEST_F( ModifiersTest, Range )
{
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

    ASSERT_EQ(f_dll.size(), f_original_size + range.size()*3);
}

TEST_F( ModifiersTest, InitializerList )
{
    constexpr auto ilist = {99,88,77,66,55,44,44,22,11};
    const auto ilist_size = std::distance(ilist.begin(),ilist.end());

    auto it1 = f_dll.insert(f_dll.cbegin(), {99,88,77,66,55,44,44,22,11});
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_TRUE(std::equal(ilist.begin(), ilist.end(), it1));

    auto it2 = f_dll.insert(f_dll.cend(), {99,88,77,66,55,44,44,22,11});
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_TRUE(std::equal(ilist.begin(),ilist.end(),it2));

    auto ipoint = f_dll.begin();
    std::advance(ipoint, f_original_size/2 + ilist_size);
    auto it3 = f_dll.insert(ipoint, {99,88,77,66,55,44,44,22,11});
    ASSERT_TRUE(ensure_invariant(f_dll));
    std::advance(ipoint, -ilist_size);
    ASSERT_EQ(ipoint, it3);
    ASSERT_TRUE(std::equal(ilist.begin(), ilist.end(), it3));
}

TEST_F( ModifiersTest, Emplace )
{
    auto lvalue = 11;
    const auto clvalue = 42;

    auto it1 = f_dll.emplace(f_dll.cbegin(), lvalue);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(*it1, lvalue);

    auto it2 = f_dll.emplace(f_dll.cend(), clvalue);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(*it2, clvalue);

    auto ipoint = f_dll.begin();
    std::advance(ipoint, f_dll.size()/2);
    auto it3 = f_dll.emplace(ipoint, lvalue+clvalue);
    ASSERT_TRUE(ensure_invariant(f_dll));
    std::advance(ipoint,-1);
    ASSERT_EQ(ipoint, it3);
    ASSERT_EQ(*ipoint, *it3);
    ASSERT_EQ(*it3, lvalue+clvalue);
}

TEST_F( ModifiersTest, EraseSingleFirst )
{
    const auto expected = *++f_dll.cbegin();
    auto it = f_dll.erase(f_dll.cbegin());
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(expected,*it);
    ASSERT_EQ(f_dll.size(), f_original_size-1);
}

// Can't use end() iterator. How do we test that?
// TEST_F( ModifiersTest, EraseSingleLast )
// {

// }

TEST_F( ModifiersTest, EraseRange )
{
    auto begin = f_dll.cbegin();
    std::advance(begin, 1);
    auto end = f_dll.cend();
    std::advance(end, -1);
    const auto range_size = std::distance(begin, end);

    auto it = f_dll.erase(begin, end);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(it, end);
    ASSERT_EQ(f_dll.size(), f_original_size - range_size);
}

TEST_F( ModifiersTest, PushBackLvalue )
{
    const auto value = 42;
    f_dll.push_back(value);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), f_original_size+1);
    ASSERT_EQ(value, f_dll.back());
}

TEST_F( ModifiersTest, PushBackRvalue )
{
    f_dll.push_back(42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), f_original_size+1);
    ASSERT_EQ(f_dll.back(), 42);
}

TEST_F( ModifiersTest, EmplaceBack )
{
    f_dll.push_back(42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), f_original_size+1);
    ASSERT_EQ(f_dll.back(), 42);
}

TEST_F( ModifiersTest, PopBack )
{
    const auto second_to_last = [&f_dll = f_dll]{ return *----f_dll.cend(); }();
    f_dll.pop_back();
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), f_original_size-1);
    ASSERT_EQ(second_to_last, f_dll.back());
}

TEST_F( ModifiersTest, PushFrontLvalue )
{
    const auto value = 42;
    f_dll.push_front(value);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.front(), value);
    ASSERT_EQ(f_dll.size(), f_original_size + 1);
}

TEST_F( ModifiersTest, PushFrontRvalue )
{
    f_dll.push_front(42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.front(), 42);
    ASSERT_EQ(f_dll.size(), f_original_size + 1);
}

TEST_F( ModifiersTest, EmplaceFront )
{
    f_dll.emplace_front(42);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.front(), 42);
    ASSERT_EQ(f_dll.size(), f_original_size + 1);
}

TEST_F( ModifiersTest, PopFront )
{
    const auto second = [&f_dll = f_dll]{ return *++f_dll.cbegin(); }();
    f_dll.pop_front();
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), f_original_size - 1);
    ASSERT_EQ(second, f_dll.front());
}

TEST_F( ModifiersTest, ResizeSmaller )
{
    const auto desired_count = 5;
    f_dll.resize(desired_count);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), desired_count);
}

TEST_F( ModifiersTest, ResizeBigger )
{
    const auto desired_count = 42;
    f_dll.resize(desired_count);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), desired_count);
}

TEST_F( ModifiersTest, ResizeEmpty )
{
    auto dll = DoubleLinkedList<int>{};
    const auto original_size = dll.size();
    ASSERT_EQ(original_size, 0);
    const auto desired_count = 42;
    dll.resize(desired_count, 42);
    ASSERT_TRUE(ensure_invariant(dll));
    ASSERT_EQ(dll.size(), desired_count);
}

TEST_F( ModifiersTest, ResizeToEmpty )
{
    const auto desired_count = 0;
    f_dll.resize(desired_count);
    ASSERT_TRUE(ensure_invariant(f_dll));
    ASSERT_EQ(f_dll.size(), desired_count);
}

TEST_F( SortTest, MergeSortEmptyList )
{
    ASSERT_TRUE(std::is_sorted(f_empty_list.cbegin(), f_empty_list.cend()));
    f_empty_list.sort_merge();
    ASSERT_TRUE(ensure_invariant(f_empty_list));
    ASSERT_TRUE(std::is_sorted(f_empty_list.cbegin(), f_empty_list.cend()));
}

TEST_F( SortTest, MergeSortSingleElem )
{
    ASSERT_TRUE(std::is_sorted(f_single_elem_list.cbegin(),
                    f_single_elem_list.cend()));
    f_single_elem_list.sort_merge();
    ASSERT_TRUE(ensure_invariant(f_single_elem_list));
    ASSERT_TRUE(std::is_sorted(f_single_elem_list.cbegin(),
                    f_single_elem_list.cend()));
}

TEST_F( SortTest, MergeSortTwoElem )
{
    ASSERT_FALSE(std::is_sorted(f_two_elem_list.cbegin(),
                    f_two_elem_list.cend()));
    std::cout << "Before sort:\n\t" << f_two_elem_list << "\n";
    f_two_elem_list.sort_merge();
    std::cout << "After sort:\n\t" << f_two_elem_list << "\n";
    ASSERT_TRUE(ensure_invariant(f_two_elem_list));
    ASSERT_TRUE(std::is_sorted(f_two_elem_list.cbegin(),
                    f_two_elem_list.cend()));
}

TEST_F( SortTest, MergeSortRandomList )
{
    ASSERT_FALSE(std::is_sorted(f_random_list.cbegin(),
                    f_random_list.cend()));
    const auto count_before = f_random_list.size();
    std::cout << "Before sort:\n\t" << f_random_list << "\n";
    f_random_list.sort_merge();
    std::cout << "After sort:\n\t" << f_random_list << "\n";
    ASSERT_TRUE(ensure_invariant(f_random_list));
    ASSERT_TRUE(std::is_sorted(f_random_list.cbegin(),
                    f_random_list.cend()));
    const auto count_after = f_random_list.size();
    ASSERT_EQ(count_before, count_after);
}

} // namespace
