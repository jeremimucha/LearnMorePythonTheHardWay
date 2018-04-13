#include "gtest/gtest.h"
#include "UnorderedMap/UnorderedMap.h"
#include <utility>
#include <string>


namespace
{


class ModifiersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(f_map.empty());
    }

    UnorderedMap<int, std::string> f_map{};
};

class ModifiersTest2 : public ::testing::Test
{
protected:
    void SetUp() override
    {
        f_map.insert(f_zero);
        f_map.insert(f_one);
        f_map.insert(f_42);
        ASSERT_FALSE(f_map.empty());
    }
    std::pair<int,std::string> f_zero{0,"Zero"};
    std::pair<int,std::string> f_one{1,"One"};
    std::pair<int,std::string> f_42{42,"Fourty Two"};
    UnorderedMap<int,std::string> f_map{};
};


TEST_F(ModifiersTest, InsertLvalue)
{
    std::pair<int,std::string> value1{0, "Zero"};
    f_map.insert(value1);
    const auto it1 = f_map.find(value1.first);
    ASSERT_EQ(it1->first, value1.first);
    ASSERT_EQ(it1->second, value1.second);
    ASSERT_TRUE(it1 != f_map.end());
    std::pair<int,std::string> value2{1,"One"};
    f_map.insert(value2);
    const auto it2 = f_map.find(value2.first);
    ASSERT_EQ(it2->first, value2.first);
    ASSERT_EQ(it2->second, value2.second);
    ASSERT_TRUE(it2 != f_map.end());
}

TEST_F(ModifiersTest, InsertRvalue)
{
    std::pair<int,std::string> value1{0, "Zero"};
    f_map.insert(std::make_pair(value1.first,value1.second));
    const auto it1 = f_map.find(value1.first);
    ASSERT_EQ(it1->first, value1.first);
    ASSERT_EQ(it1->second, value1.second);
    ASSERT_TRUE(it1 != f_map.end());
    std::pair<int,std::string> value2{1,"One"};
    f_map.insert(std::make_pair(value2.first,value2.second));
    const auto it2 = f_map.find(value2.first);
    ASSERT_EQ(it2->first, value2.first);
    ASSERT_EQ(it2->second, value2.second);
    ASSERT_TRUE(it2 != f_map.end());
}

TEST_F(ModifiersTest2, OperatorBrackets)
{
    ASSERT_EQ(f_42.second, f_map[f_42.first]);
    f_map[f_42.first] = "Foo";
    ASSERT_EQ(f_map[f_42.first], "Foo");
    ASSERT_EQ(f_map[99], "");
    f_map[11] = "Eleven";
    ASSERT_EQ(f_map[11], "Eleven");
}


} // namespace
