#include <string>
#include <gtest/gtest.h>

#include "Translator.h"


using namespace std::string_literals;
namespace
{


class TranslatorTests : public ::testing::Test {
protected:
    void SetUp() override { }
};


TEST_F(TranslatorTests, Ctor)
{
    const auto str1 = "ABC"s;
    const auto str2 = "123"s;
    const auto tr = Translator{str1, str2};

    const auto tr2 = Translator{"one", "two"};

    const auto tr3 = Translator{"abc"s, "987"};
}

TEST_F(TranslatorTests, Translate)
{
    const auto str1 = "ABCDE"s;
    const auto str2 = "123"s;
    auto tr = Translator{str1, str2};
    ASSERT_EQ(tr.translate(str1[0]), str2[0]);
    ASSERT_EQ(tr.translate(str1[1]), str2[1]);
    ASSERT_EQ(tr.translate(str1[2]), str2[2]);
    ASSERT_EQ(tr.translate(str1[3]), str2[2]);
    ASSERT_EQ(tr.translate(str1[4]), str2[2]);

    ASSERT_EQ(tr.translate('Z'), 'Z');
}

TEST_F(TranslatorTests, Truncate)
{
    const auto str1 = "ABCDE"s;
    const auto str2 = "123"s;
    auto tr = Translator{str1, str2};
    tr.truncate();
    ASSERT_EQ(tr.translate(str1[0]), str2[0]);
    ASSERT_EQ(tr.translate(str1[1]), str2[1]);
    ASSERT_EQ(tr.translate(str1[2]), str2[2]);

    ASSERT_EQ(tr.translate(str1[3]), str1[3]);
    ASSERT_EQ(tr.translate(str1[4]), str1[4]);
    ASSERT_EQ(tr.translate('Z'), 'Z');
}

} // namespace

