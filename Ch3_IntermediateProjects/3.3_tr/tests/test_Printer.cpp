#include "gtest/gtest.h"
#include <string>
#include <sstream>
#include "Printer.h"

using namespace std::literals::string_literals;

namespace
{


class PrinterTest : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    const Hex_chunk chunk{
        {0x42,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
         0x2e,0x2f,0x4a,0x45,0x52,0x45,0x4d,0x49}, // buffer
         16,                                       // count
         0xff                                      // offset
    };

    std::ostringstream oss;
};


TEST_F(PrinterTest, BaseTest)
{
    const auto expected_base = std::string{
        "000000ff  66 37 38 39 40 41 42 43 46 47 74 69 82 69 77 73 "
    };
    auto test_stream = Printer_base(oss);
    test_stream << chunk;
    ASSERT_EQ(oss.str(), expected_base);
}

TEST_F(PrinterTest, HexTest)
{
    const auto expected_hex =
        "000000ff  42 25 26 27 28 29 2a 2b 2e 2f 4a 45 52 45 4d 49 "s;
    auto test_stream = Printer_hex(oss);
    test_stream << chunk;
    ASSERT_EQ(oss.str(), expected_hex);
}

TEST_F(PrinterTest, OctalTest)
{
    const auto expected_octal =
        "000000ff  102 045 046 047 050 051 052 053 056 057 112 105 122 105 115 111 "s;
    auto test_stream = Printer_octal(oss);
    test_stream << chunk;
    ASSERT_EQ(oss.str(), expected_octal);
}

TEST_F(PrinterTest, CanonicalTest)
{
    const auto expected_canon =
        "000000ff  42 25 26 27 28 29 2a 2b  2e 2f 4a 45 52 45 4d 49  |B%&'()*+./JEREMI|"s;
    auto test_stream = Printer_canonical(oss);
    test_stream << chunk;
    ASSERT_EQ(oss.str(), expected_canon);
}

TEST_F(PrinterTest, CharTest)
{
    const auto expected_ascii =
        "000000ff   B  %  &  '  (  )  *  +  .  /  J  E  R  E  M  I "s;
    auto test_stream = Printer_char(oss);
    test_stream << chunk;
    ASSERT_EQ(oss.str(), expected_ascii);
}

} // namespace
