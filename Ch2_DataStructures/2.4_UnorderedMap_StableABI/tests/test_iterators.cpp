#include "gtest/gtest.h"
#include <string>
#include <iostream>


#include "UnorderedMap/UnorderedMap.h"


namespace
{


class IteratorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};


TEST_F(IteratorTest, SingleElem)
{
    UnorderedMap<int, std::string> map{};
    const auto value = std::make_pair(42, "Fourty Two");
    map.insert(value);
    ASSERT_FALSE(map.empty());
    ASSERT_EQ(1, map.size());
    auto begin = map.begin();
    auto cbegin = map.cbegin();
    ASSERT_EQ(begin, cbegin);
    auto end = map.end();
    auto cend = map.cend();
    ASSERT_EQ(end, cend);
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_EQ(begin, end);
}

TEST_F(IteratorTest, TwoElems)
{
    UnorderedMap<int, std::string> map{};
    const auto value1 = std::make_pair(42, "Fourty Two");
    const auto value2 = std::make_pair(0, "Zero");
    map.insert(value1);
    map.insert(value2);
    // assert_invariant(map);
    ASSERT_FALSE(map.empty());
    ASSERT_EQ(2, map.size());
    auto begin = map.begin();
    auto cbegin = map.cbegin();
    ASSERT_EQ(begin, cbegin);
    auto end = map.end();
    auto cend = map.cend();
    ASSERT_EQ(end, cend);
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_NE(begin, end);
    ++begin;
    ASSERT_EQ(begin, end);
}

TEST_F(IteratorTest, 1kElems)
{
    UnorderedMap<int, std::string> map{};
    for(int i=0; i<1000; ++i){
        map.insert({i,std::to_string(i)});
    }
    // assert_invariant(map);
    auto begin = map.begin();
    auto end = map.end();
    ASSERT_NE(begin, end);
    while(++begin != end)
        ;
    ASSERT_EQ(begin, end);
}

} // namespace
