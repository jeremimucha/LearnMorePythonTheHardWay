#include <string>
#include <unordered_map>
#include "gtest/gtest.h"
#include "TernaryTree/TernaryTree.h"


namespace
{

using namespace std::literals::string_literals;

class LookupTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        f_tree.insert(k1,v1);
    }

    TernaryTree<std::string,int> f_tree{};
    const std::string k1{"FourtyTwo"};
    const int v1{42};
    const std::string k2{"Eleven"};
    const int v2{11};
};

TEST_F(LookupTest, FindValueSingle)
{
    const auto found = f_tree.find_value(k1);
    ASSERT_EQ(found, v1);
}

TEST_F(LookupTest, FindValueTwo)
{
    f_tree.insert(k2,v2);
    const auto found_k1 = f_tree.find_value(k1);
    ASSERT_EQ(found_k1, v1);
    const auto found_k2 = f_tree.find_value(k2);
    ASSERT_EQ(found_k2, v2);
}

TEST_F(LookupTest, Find10)
{
    const std::unordered_map<int, std::string> map{
        {0, "Zero"}
       ,{1, "One"}
       ,{2, "Two"}
       ,{3, "Three"}
       ,{4, "Four"}
       ,{5, "Five"}
       ,{6, "Six"}
       ,{7, "Seven"}
       ,{8, "Eight"}
       ,{9, "Nine"}
       ,{10,"Ten"}
    };

    for( const auto& kv : map )
        f_tree.insert(kv.second, kv.first);

    ASSERT_FALSE(f_tree.empty());

    for( const auto& kv : map )
        ASSERT_EQ(f_tree.find_value(kv.second), kv.first);
}

} // namespace
