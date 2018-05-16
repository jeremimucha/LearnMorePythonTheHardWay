#include <iostream>

#include "gtest/gtest.h"
#include "SuffixArray/SuffixArray.h"


namespace
{


class FindTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        std::cout << f_suffix_array << std::endl;
    }

    const SuffixArray f_suffix_array{"abracadabra"};
};


TEST_F(FindTest, findShortest)
{
    const auto it = f_suffix_array.find_shortest("abra");
    ASSERT_NE(it, f_suffix_array.cend());
    ASSERT_EQ(*it, "abra");
}

TEST_F(FindTest, findLongest)
{
    const auto it = f_suffix_array.find_longest("abra");
    ASSERT_NE(it, f_suffix_array.cend());
    ASSERT_EQ(*it, "abracadabra");
}

TEST_F(FindTest, findAll)
{
    const auto expected = typename SuffixArray::suffix_vector_t{"abra", "abracadabra"};
    const auto result = f_suffix_array.find_all("abra");
    ASSERT_EQ(expected, result);
}


} // namespace
