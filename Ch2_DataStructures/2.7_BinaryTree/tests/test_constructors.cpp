#include "gtest/gtest.h"
#include "BinaryTree/BinaryTree.h"


namespace
{


class ConstructorTest : public ::testing::Test
{
    void SetUp() override
    {
    }

    BinaryTree<int, std::string> f_tree{};
};

TEST_F(ConstructorTest, DefaultConstructor)
{
    ASSERT_TRUE(true);
}


} // namespace
