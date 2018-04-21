#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <utility>
#include <functional>

#include "UnorderedMap/UnorderedMap.h"


namespace
{


class InsertTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        assert_invariant(f_map);
        ASSERT_TRUE(f_map.empty());
    }

    UnorderedMap<int,std::string> f_map{};
};

class EraseTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        for(int i=0; i<42; ++i){
            f_map.insert({i,std::to_string(i)});
        }
        f_map.insert(value1);
        f_map.insert(value2);

        assert_invariant(f_map);
        ASSERT_EQ(f_map.size(), 44);
        ASSERT_NE(f_map.find(value1.first), f_map.cend());
        ASSERT_NE(f_map.find(value2.first), f_map.cend());
    }

    UnorderedMap<int,std::string> f_map{};
    std::pair<int,std::string> value1{42, "Fourty Two"};
    std::pair<int,std::string> value2{144, "One hundred fourty four"};
};


TEST_F( InsertTest, insertLvalue )
{
    const auto value1 = std::make_pair(42, "Fourty Two");
    const auto value2 = std::make_pair(0, "Zero");
    f_map.insert(value1);
    f_map.insert(value2);
    assert_invariant(f_map);
    ASSERT_EQ(2, f_map.size());
    auto it1 = std::find_if(f_map.cbegin(), f_map.cend(),
                [&value1](const auto& val){return value1.first == val.first;});
    ASSERT_NE(it1, f_map.cend());
    ASSERT_EQ(it1->first, value1.first);
    ASSERT_EQ(it1->second, value1.second);
    auto it2 = std::find_if(f_map.cbegin(), f_map.cend(),
                [&value2](const auto& val){return value2.first == val.first;});
    ASSERT_NE(it2, f_map.cend());
    ASSERT_EQ(it2->first, value2.first);
    ASSERT_EQ(it2->second, value2.second);
    assert_invariant(f_map);
}

TEST_F( InsertTest, insertRvalue )
{
    const auto value1 = std::make_pair(42, "Fourty Two");
    const auto value2 = std::make_pair(0, "Zero");
    f_map.insert(std::make_pair(value1.first, value1.second));
    f_map.insert(std::make_pair(value2.first,value2.second));
    assert_invariant(f_map);
    auto it1 = std::find_if(f_map.cbegin(), f_map.cend(),
                [&value1](const auto& val){return value1.first == val.first;});
    ASSERT_NE(it1, f_map.cend());
    ASSERT_EQ(it1->first, value1.first);
    ASSERT_EQ(it1->second, value1.second);
    auto it2 = std::find_if(f_map.cbegin(), f_map.cend(),
                [&value2](const auto& val){return value2.first == val.first;});
    ASSERT_NE(it2, f_map.cend());
    ASSERT_EQ(it2->first, value2.first);
    ASSERT_EQ(it2->second, value2.second);
    assert_invariant(f_map);
}

TEST_F( EraseTest, eraseByKey )
{
    f_map.erase(value1.first);
    const auto it1 = f_map.find(value1.first);
    const auto stdit1 = std::find_if(f_map.cbegin(), f_map.cend(),
                [&key=value1.first](const auto& val){return val.first==key;});
    const auto end = f_map.end();
    ASSERT_EQ(it1, stdit1);
    ASSERT_EQ(it1, end);
    ASSERT_EQ(stdit1, end);
    assert_invariant(f_map);
    f_map.erase(value2.first);
    ASSERT_EQ(f_map.find(value2.first), f_map.cend());
    assert_invariant(f_map);
}

TEST_F( EraseTest, eraseByIterator )
{
    f_map.erase(f_map.find(value1.first));
    const auto it1 = f_map.find(value1.first);
    ASSERT_EQ(it1, f_map.end());
    assert_invariant(f_map);
    f_map.erase(f_map.find(value2.first));
    const auto it2 = f_map.find(value2.first);
    ASSERT_EQ(it2, f_map.end());
    ASSERT_EQ(44-2, f_map.size());
    assert_invariant(f_map);
}

TEST_F( EraseTest, eraseRange )
{
    auto it1 = f_map.find(value1.first);
    auto it2 = f_map.find(value2.first);
    if(std::hash<int>{}(it1->first) % 19 
        < std::hash<int>{}(it2->first) %19){
        f_map.erase(it1, it2);
        ASSERT_EQ(f_map.find(value1.first), f_map.end());
    } else {
        f_map.erase(it2, it1);
        ASSERT_EQ(f_map.find(value2.first), f_map.end());
    }
    assert_invariant(f_map);
}

} // namespace
