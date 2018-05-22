#include "gtest/gtest.h"
#include "TernaryTree/TernaryTree.h"


namespace
{

using namespace std::literals::string_literals;

class ModifiersTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(f_tree.empty());
    };

    TernaryTree<std::string, int> f_tree{};
};

TEST_F(ModifiersTest, InsertSingle)
{
    const auto key   = "Fourty Two"s;
    constexpr int  value = 42;
    f_tree.insert(key, value);
    ASSERT_FALSE(f_tree.empty());
}

TEST_F(ModifiersTest, InsertTwo)
{
    const auto key1 = "Fourty Two"s;
    constexpr int value1 = 42;
    const auto key2 = "Eleven";
    constexpr int value2 = 11;
    f_tree.insert(key1, value1);
    f_tree.insert(key2, value2);
    ASSERT_FALSE(f_tree.empty());
}

} // namespace
