#include <iostream>

#include "gtest/gtest.h"
#include "SuffixArray/SuffixArray.h"


namespace
{


class FindTest : public ::testing::Test {
protected:
    void SetUp() override
    {
    }

    const SuffixArray f_suffix_array{"abracadabra"};
};


TEST_F(FindTest, findShortest)
{
    const auto it = f_suffix_array.find_shortest("abra");
    ASSERT_FALSE(it.empty()) << "Didn't find abra in: " << f_suffix_array << "\n";
    ASSERT_EQ(it, "abra");
}

TEST_F(FindTest, findLongest)
{
    const auto it = f_suffix_array.find_longest("abra");
    ASSERT_FALSE(it.empty()) << "Didn't find abra in: " << f_suffix_array << "\n";
    ASSERT_EQ(it, "abracadabra");
}

TEST_F(FindTest, findAll)
{
    auto expected = typename std::vector<std::string>{"abra","abracadabra"};
    const auto result = f_suffix_array.find_all("abra");
    ASSERT_TRUE(result.empty()) << "Didn't find suffixes starting with 'abra' in: "
        << f_suffix_array << "\n";
    ASSERT_EQ(expected, result);
}


} // namespace
