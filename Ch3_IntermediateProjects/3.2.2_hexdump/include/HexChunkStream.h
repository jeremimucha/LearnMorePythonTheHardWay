#pragma once

#include <istream>
#include "HexChunk.h"

class Hex_chunk_stream
{
public:
    explicit Hex_chunk_stream(std::istream& is) noexcept
        : is_{is.rdbuf()} { }

    explicit operator bool() const noexcept { return done_ < 2; }

friend Hex_chunk_stream& operator>>(Hex_chunk_stream& strm, Hex_chunk& chunk) noexcept
{
    chunk.offset = strm.offset_;
    strm.is_.read(chunk.buffer, Hex_chunk::MaxChunk);
    chunk.n = strm.is_.gcount();
    strm.offset_ += chunk.n;
    if(chunk.n == 0 && strm.is_.eof()) ++strm.done_;
    return strm;
}
private:
    offset_t offset_{0};
    std::istream is_;
    int done_{0};
};

// class Hex_chunk_fstream
// {
// public:
//     explicit Hex_chunk_fstream(const std::string& fname)
//         : ifs_{fname} { }
//     explicit operator bool() const noexcept { return ifs_.good(); }

// friend Hex_chunk_fstream& operator>>(Hex_chunk_fstream& fstrm, Hex_chunk& chunk) noexcept
// {
//     fstrm.ifs_.read(chunk.buffer, Hex_chunk::MaxChunk);
//     chunk.n = fstrm.ifs_.gcount();
//     chunk.offset += chunk.n;
//     return fstrm;
// }

// private:
//     std::ifstream ifs_;
// };
