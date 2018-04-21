#include "gtest/gtest.h"
#include <string>
#include <iostream>
#include <utility>

#include "UnorderedMap/UnorderedMap.h"


namespace
{


class FindTest : public ::testing::Test {
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


TEST_F( FindTest, byKey )
{
    const auto it1a = f_map.find(value1.first);
    ASSERT_NE(it1a, f_map.end());
    const auto it1b = std::find_if(f_map.cbegin(), f_map.cend(),
                            [&key=value1.first](const auto& value)
                            {return value.first == key;});
    ASSERT_EQ(it1a, it1b);
    const auto it2a = f_map.find(value2.first);
    ASSERT_NE(it2a, f_map.end());
    const auto it2b = std::find_if(f_map.cbegin(), f_map.cend(), 
                        [&key=value2.first](const auto& value)
                        {return value.first == key;});
    ASSERT_EQ(it2a, it2b);
    assert_invariant(f_map);
}


} // namespace
