#include "gtest/gtest.h"
#include <string>
#include "TernaryTree/TernaryTree.h"


namespace
{

class ConstructorTest : public ::testing::Test
{ };

TEST_F(ConstructorTest, DefaultCtor)
{
    TernaryTree<std::string, int> f_tree{};
}

} // namespace
