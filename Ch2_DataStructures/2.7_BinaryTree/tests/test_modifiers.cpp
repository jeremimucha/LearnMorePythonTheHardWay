#include <chrono>
#include <random>
#include "gtest/gtest.h"
#include "BinaryTree/BinaryTree.h"


namespace
{

std::uniform_int_distribution<> ud;
std::default_random_engine re{
    static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())};


class ModifiersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(f_tree.empty());
        ASSERT_EQ(0, f_tree.size());
    }

    BinaryTree<int, std::string> f_tree{};
};

TEST_F(ModifiersTest, InsertTest)
{
    const auto value1 = std::make_pair(42, "Fourty Two");
    const auto res1 = f_tree.insert(value1);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(1, f_tree.size());
    // const auto res1 = f_tree.find(value1.first);
    // ASSERT_NE(res1.first, nullptr);
    ASSERT_TRUE(res1.second);
    ASSERT_EQ(res1.first->first, value1.first);
    ASSERT_EQ(res1.first->second, value1.second);

    const auto value2 = std::make_pair(11, "Eleven");
    const auto res2 = f_tree.insert(value2);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(2, f_tree.size());
    // const auto res2 = f_tree.find(value2.first);
    // ASSERT_NE(res2.first, nullptr);
    ASSERT_TRUE(res2.second);
    ASSERT_EQ(res2.first->first, value2.first);
    ASSERT_EQ(res2.first->second, value2.second);

    const auto value3 = std::make_pair(81, "81");
    const auto res3 = f_tree.insert(value3);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(3, f_tree.size());
    // const auto res3 = f_tree.find(value3.first);
    // ASSERT_NE(res3.first, nullptr);
    ASSERT_TRUE(res3.second);
    ASSERT_EQ(res3.first->first, value3.first);
    ASSERT_EQ(res3.first->second, value3.second);
}

TEST_F(ModifiersTest, InsertResultTest)
{
    for(int i=0; i<100; ++i){
        const auto num = ud(re);
        f_tree.insert({num, std::to_string(num)});
    }
    const auto value = std::make_pair(42, "Fourty Two");
    auto res = f_tree.insert(value);
    ASSERT_TRUE(res.second);
    auto it = res.first;
    auto endit = f_tree.end();
    ASSERT_NE(it, endit);
    auto count = unsigned{0};
    for(; it!=endit; ++it)
        ++count;
    ASSERT_NE(count, 0u);
}

} // namespace
