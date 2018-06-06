#include "gtest/gtest.h"
#include <sstream>
#include <string>
#include <algorithm>

#include "HexChunkStream.h"

using namespace std::string_literals;

bool operator==(const Hex_chunk& lhs, const Hex_chunk& rhs)
{
    auto lhs_begin = std::begin(lhs.buffer);
    auto lhs_end = lhs_begin;
    std::advance(lhs_end, lhs.n);
    auto rhs_begin = std::begin(rhs.buffer);
    return lhs.n == rhs.n && lhs.offset == rhs.offset &&
        std::equal(lhs_begin, lhs_end, rhs_begin);
}

void PrintTo(const Hex_chunk& chunk, ::std::ostream* os)
{
    *os << "n=" << chunk.n << ", offset=" << chunk.offset << "{";
    for(int i=0; i<chunk.n; ++i)
        *os << chunk.buffer[i] << ",";
    *os << "}";
}

namespace
{

class TestHexChunkStream : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
    Hex_chunk chunk{{0},0,0};
};


TEST_F(TestHexChunkStream, Exact)
{
    std::istringstream source_{"B%&'()*+./JEREMI"};
    const Hex_chunk expected{
        {0x42,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
         0x2e,0x2f,0x4a,0x45,0x52,0x45,0x4d,0x49}, // buffer
         16,                                       // count
         0x00                                      // offset
    };
    auto test_stream = Hex_chunk_stream{source_};
    test_stream >> chunk;
    ASSERT_EQ(chunk, expected);
}

TEST_F(TestHexChunkStream, Partial)
{
    std::istringstream source_{"B%&'()*+./"};
    const Hex_chunk expected{
        {0x42,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
         0x2e,0x2f}, // buffer
         10,                                       // count
         0x00                                      // offset
    };
    auto test_stream = Hex_chunk_stream{source_};
    test_stream >> chunk;
    ASSERT_EQ(chunk, expected);
}

TEST_F(TestHexChunkStream, LongerInput)
{
    std::istringstream source_{"B%&'()*+./JEREMIB%&'()*+./JEREMI"};
    Hex_chunk expected{
        {0x42,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
         0x2e,0x2f,0x4a,0x45,0x52,0x45,0x4d,0x49}, // buffer
         16,                                       // count
         0x00                                      // offset
    };
    auto test_stream = Hex_chunk_stream{source_};
    test_stream >> chunk;
    ASSERT_EQ(chunk, expected);
    
    expected.offset = 16;
    test_stream >> chunk;
    ASSERT_EQ(chunk, expected);

    expected.n = 0;
    expected.offset = 32;
    test_stream >> chunk;
    ASSERT_EQ(chunk, expected);

    test_stream >> chunk;
    ASSERT_FALSE(test_stream);
}

} // namespace
