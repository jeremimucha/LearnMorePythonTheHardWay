#include "gtest/gtest.h"
#include "UnorderedMap/UnorderedMap.h"
#include <string>


namespace
{


class ConstructorTest : public ::testing::Test
{
    void SetUp() override
    {
        ASSERT_TRUE(f_map.empty());
    }
    UnorderedMap<int,std::string> f_map{};
};


TEST_F(ConstructorTest, Dummy)
{
    ASSERT_TRUE(true);
}


} // namespace
