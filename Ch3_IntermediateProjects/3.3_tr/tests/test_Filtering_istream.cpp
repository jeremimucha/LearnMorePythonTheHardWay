#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "Filtering_istream.h"


namespace
{

using namespace std::string_literals;

class TestFiltering_istream : public ::testing::Test {
protected:
    void SetUp() override { }

    std::string dset{"ABC"};
    std::string sqset{"xyz"};
    std::istringstream iss{"AByyyyyzzzzzAzzBBxxzzyyCCC"};
    Filtering_istream fis{iss};
    std::ostringstream oss{};
};

TEST_F(TestFiltering_istream, DelTest)
{
    fis.del_set(dset);
    fis.del(true);
    for( char ch; fis.get(ch); )
        oss << ch;
    ASSERT_EQ(oss.str(), "yyyyyzzzzzzzxxzzyy"s);
}

TEST_F(TestFiltering_istream, SqueezeTest)
{
    fis.squeeze_set(sqset);
    fis.squeeze(true);
    for( char ch; fis.get(ch); )
        oss << ch;
    ASSERT_EQ(oss.str(), "AByzAzBBxzyCCC");
}

TEST_F(TestFiltering_istream, SqueezeDeleteTest)
{
    fis.squeeze_set(sqset);
    fis.squeeze(true);
    fis.del_set(dset);
    fis.del(true);
    for( char ch; fis.get(ch); )
        oss << ch;
    ASSERT_EQ(oss.str(), "yzzxzy");
}

} // namespace
