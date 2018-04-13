#include "gtest/gtest.h"
#include "UnorderedMap/UnorderedMap.h"


namespace
{


class ConstructorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        f_map.insert({0,"Zero"});
        f_map.insert({1,"One"});
        f_map.insert({2,"Two"});

        ASSERT_TRUE(std::find_if(f_map.cbegin(), f_map.cend(),
                    [](const auto& val){return val.first == 0 && val.second=="Zero";})
                    != f_map.end());
    }

    UnorderedMap<int, std::string> f_map{};
};

TEST_F(ConstructorTest, Dummy)
{
    ASSERT_TRUE(true);
}

} // namespace
