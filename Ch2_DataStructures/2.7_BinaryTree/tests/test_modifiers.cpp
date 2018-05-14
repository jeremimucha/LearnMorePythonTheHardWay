#include <chrono>
#include <random>
#include <utility>
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
    std::pair<int,std::string> v1{1,"One"};
    std::pair<int,std::string> v2{2,"Two"};
    std::pair<int,std::string> v3{3,"Three"};
    std::pair<int,std::string> v4{4,"Four"};
    std::pair<int,std::string> v5{5,"Five"};
    std::pair<int,std::string> v6{6,"Six"};
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

TEST_F(ModifiersTest, EraseFromEmpty)
{
    const auto it = f_tree.erase(42);
    ASSERT_EQ(it, f_tree.end());
    ASSERT_TRUE(f_tree.empty());
    ASSERT_EQ(0u, f_tree.size());
}

TEST_F(ModifiersTest, EraseLeaf)
{
    auto value1 = std::make_pair(42, "Fourty Two");
    auto value2 = std::make_pair(11, "Eleven");
    f_tree.insert(value1);
    f_tree.insert(value2);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(2u, f_tree.size());
    const auto it = f_tree.erase(value2.first);
    ASSERT_EQ(it, f_tree.end());
    const auto fit = f_tree.find(value2.first);
    ASSERT_EQ(fit, f_tree.end());
}

TEST_F(ModifiersTest, EraseLeafRoot)
{
    f_tree.insert(v1);
    const auto it = f_tree.erase(v1.first);
    ASSERT_EQ(it, f_tree.end());
    const auto fit = f_tree.find(v1.first);
    ASSERT_EQ(fit, f_tree.end());
    ASSERT_TRUE(f_tree.empty());
    ASSERT_EQ(0u, f_tree.size());
}

TEST_F(ModifiersTest, EraseSemiLeaf)
{
    f_tree.insert(v5);
    f_tree.insert(v4);
    f_tree.insert(v3);
    ASSERT_FALSE(f_tree.empty());
    ASSERT_EQ(3u, f_tree.size());
    const auto found4 = f_tree.find(v4.first);
    ASSERT_NE(found4, f_tree.end());
    ASSERT_EQ(found4->first, v4.first);
    ASSERT_EQ(found4->second, v4.second);
    const auto found3 = f_tree.find(v3.first);
    ASSERT_NE(found3, f_tree.end());
    ASSERT_EQ(found3->first, v3.first);
    ASSERT_EQ(found3->second, v3.second);
    auto res = f_tree.erase(v4.first);
    ASSERT_EQ(res, found3);
}

TEST_F(ModifiersTest, EraseSemiLeafRoot)
{
    f_tree.insert(v4);
    f_tree.insert(v2);
    f_tree.insert(v3);
    f_tree.insert(v1);
    const auto found = f_tree.find(v4.first);
    ASSERT_EQ(found, f_tree.begin());
    const auto found2 = f_tree.find(v2.first);
    const auto it = f_tree.erase(v4.first);
    ASSERT_EQ(it, found2);
    ASSERT_EQ(it, f_tree.begin());
}

TEST_F(ModifiersTest, EraseBranch)
{
    f_tree.insert(v2);
    f_tree.insert(v4); // erase target
    f_tree.insert(v3);
    f_tree.insert(v5);
    f_tree.insert(v6);
    const auto found4 = f_tree.find(v4.first);
    ASSERT_NE(found4, f_tree.end());
    const auto successor = f_tree.find(v5.first);
    const auto it = f_tree.erase(v4.first);
    ASSERT_EQ(it, successor);
    ASSERT_EQ(4u, f_tree.size());
}

TEST_F(ModifiersTest, EraseBranch2)
{
    f_tree.insert(v2);
    f_tree.insert(v4); // erase target
    f_tree.insert(v3);
    f_tree.insert(v6);
    f_tree.insert(v5);
    const auto found4 = f_tree.find(v4.first);
    ASSERT_NE(found4, f_tree.end());
    const auto successor = f_tree.find(v5.first);
    const auto it = f_tree.erase(v4.first);
    ASSERT_EQ(it, successor);
    ASSERT_EQ(4u, f_tree.size());
}

TEST_F(ModifiersTest, EraseBranchRoot)
{
    f_tree.insert(v3); // erase target
    f_tree.insert(v5);
    f_tree.insert(v1);
    f_tree.insert(v6);
    f_tree.insert(v4); 
    f_tree.insert(v2);
    const auto found3 = f_tree.find(v3.first);
    ASSERT_EQ(found3, f_tree.begin());
    const auto it = f_tree.erase(v3.first);
    ASSERT_EQ(it->first, v4.first);
    ASSERT_EQ(5u, f_tree.size());
}

TEST_F(ModifiersTest, EraseNonExistent)
{
    f_tree.insert(v3);
    f_tree.insert(v5);
    f_tree.insert(v1);
    f_tree.insert(v6);
    f_tree.insert(v4); 
    f_tree.insert(v2);
    const auto it = f_tree.erase(42);
    ASSERT_EQ(it, f_tree.end());
    ASSERT_EQ(6u, f_tree.size());
}

} // namespace
