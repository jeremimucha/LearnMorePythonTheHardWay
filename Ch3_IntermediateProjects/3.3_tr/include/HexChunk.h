#pragma once

#include <iosfwd>

using byte = char;
using offset_t = std::streamsize;

struct Hex_chunk
{
    static constexpr int MaxChunk{16};
    byte buffer[MaxChunk];
    int n;
    offset_t offset;
};

// struct Hex_chunk_hex : public Hex_chunk_base { };
// struct Hex_chunk_octal : public Hex_chunk_base { };
// struct Hex_chunk_char : public Hex_chunk_base { };
// struct Hex_chunk_canonical : public Hex_chunk_base { };
