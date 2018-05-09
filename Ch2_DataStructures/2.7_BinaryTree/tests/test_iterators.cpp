#include "gtest/gtest.h"
#include <random>
#include <chrono>
#include "BinaryTree/BinaryTree.h"


namespace
{

using namespace std::literals::chrono_literals;

std::uniform_int_distribution<> ud;
std::default_random_engine re{
    static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};

class IteratorTest : public ::testing::Test
{
protected:

    BinaryTree<int, std::string> f_tree{};
};

class InorderIteratorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for(int i=0; i<1000; ++i){
            const auto num = ud(re);
            f_tree.insert({num, std::to_string(num)});
        }
        ASSERT_FALSE(f_tree.empty());
        ASSERT_EQ(1000u, f_tree.size());
    }

    BinaryTree<int, std::string> f_tree{};
};

class PostorderIteratorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for(int i=0; i<1000; ++i){
            const auto num = ud(re);
            f_tree.insert({num, std::to_string(num)});
        }
        ASSERT_FALSE(f_tree.empty());
        ASSERT_EQ(1000u, f_tree.size());
    }

    BinaryTree<int, std::string> f_tree{};
};


TEST_F(IteratorTest, SingleElem)
{
    const auto value = std::make_pair(42, "Fourty Two");
    f_tree.insert(value);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(1u, f_tree.size());
    auto begin = f_tree.begin();
    auto cbegin = f_tree.cbegin();
    ASSERT_EQ(begin, cbegin);
    auto end = f_tree.end();
    auto cend = f_tree.cend();
    ASSERT_EQ(end, cend);
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_EQ(begin, end);
}

TEST_F(IteratorTest, TwoElems)
{
    const auto value1 = std::make_pair(42, "Fourty Two");
    const auto value2 = std::make_pair(0, "Zero");
    f_tree.insert(value1);
    f_tree.insert(value2);
    // assert_invariant(f_tree);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(2u, f_tree.size());
    auto begin = f_tree.begin();
    auto cbegin = f_tree.cbegin();
    ASSERT_EQ(begin, cbegin);
    auto end = f_tree.end();
    auto cend = f_tree.cend();
    ASSERT_EQ(end, cend);
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_EQ(begin, end);
}

TEST_F(IteratorTest, 1kElems)
{
    for(int i=0; i<1000; ++i){
        const auto num = ud(re);
        f_tree.insert({num,std::to_string(num)});
    }
    // assert_invariant(f_tree);
    auto begin = f_tree.begin();
    auto end = f_tree.end();
    ASSERT_NE(begin, end);
    while(++begin != end)
        ;
    ASSERT_EQ(begin, end);
}

TEST_F(InorderIteratorTest, InteropTest)
{
    auto it1 = f_tree.inorder_begin();
    auto cit1 = f_tree.inorder_cbegin();
    ASSERT_EQ(it1, cit1);
    auto end1 = f_tree.inorder_end();
    ASSERT_NE(it1, end1);
}

TEST_F(InorderIteratorTest, OrderTest)
{
    auto cit1 = f_tree.inorder_cbegin();
    auto end1 = f_tree.inorder_cend();
    auto previous = *cit1;
    ++cit1;
    for(; cit1 != end1; ++cit1){
        ASSERT_LT(previous.first, cit1->first);
    }
    ASSERT_EQ(cit1, end1);
}

TEST_F(PostorderIteratorTest, InteropTest)
{
    auto it1 = f_tree.postorder_begin();
    auto cit1 = f_tree.postorder_cbegin();
    ASSERT_EQ(it1, cit1);
    auto end1 = f_tree.postorder_end();
    ASSERT_NE(it1, end1);
}

TEST_F(PostorderIteratorTest, OrderTest)
{
    auto cit1 = f_tree.postorder_cbegin();
    auto end1 = f_tree.postorder_cend();
    auto previous = *cit1;
    ++cit1;
    for(; cit1 != end1; ++cit1){
        ASSERT_GT(previous.first, cit1->first);
    }
    ASSERT_EQ(cit1, end1);
}

} // namespace
