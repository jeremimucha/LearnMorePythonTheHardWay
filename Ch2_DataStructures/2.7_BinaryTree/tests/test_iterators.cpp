#include "gtest/gtest.h"
#include "BinaryTree/BinaryTree.h"


namespace
{


class IteratorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
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
        f_tree.insert({i,std::to_string(i)});
    }
    // assert_invariant(f_tree);
    auto begin = f_tree.begin();
    auto end = f_tree.end();
    ASSERT_NE(begin, end);
    while(++begin != end)
        ;
    ASSERT_EQ(begin, end);
}

} // namespace
