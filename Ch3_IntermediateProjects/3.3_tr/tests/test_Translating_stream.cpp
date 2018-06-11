#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "Translating_stream.h"


namespace
{

using namespace std::string_literals;

class TestTranslating_stream : public ::testing::Test {
protected:
    void SetUp() override { }

std::ostringstream oss;
const std::string set1{"ABC"};
const std::string set2{"123"};
Translating_stream tros{oss, {set1,set2}};
};

TEST_F(TestTranslating_stream, Ctor)
{
    const auto tros = Translating_stream{oss, {"abc"s, "123"}};
}

TEST_F(TestTranslating_stream, CharOutput)
{
    tros << 'A' << 'B' << 'C';
    ASSERT_EQ(oss.str(), set2);

    oss.clear();
    oss.seekp(0);
    tros << 'x' << 'y' << 'z';
    ASSERT_EQ(oss.str(), "xyz"s);
}

TEST_F(TestTranslating_stream, StringOutput)
{
    tros << "ABC"s;
    ASSERT_EQ(oss.str(), "123"s);

    oss.clear();
    oss.seekp(0);
    tros << "xyzA"s;
    ASSERT_EQ(oss.str(), "xyz1"s);
}

TEST_F(TestTranslating_stream, ConvertibleToStrOutput)
{
    tros << "ABC";
    ASSERT_EQ(oss.str(), "123"s);

    oss.clear();
    oss.seekp(0);
    tros << "xyzA";
    ASSERT_EQ(oss.str(), "xyz1"s);
}

} // namespace