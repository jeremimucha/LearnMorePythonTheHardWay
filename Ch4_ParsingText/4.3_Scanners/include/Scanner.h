#pragma once

#include "Token.h"
#include <istream>


class Scanner
{
public:
    using source_t = std::istream;
    Scanner(source_t& src)
        : source_{src}, full_{false}, buffer_{0}
        { }
    Scanner(const Scanner&) = delete;
    Scanner& operator=(const Scanner&) = delete;
    Scanner(Scanner&&) noexcept = default;
    Scanner& operator=(Scanner&&) noexcept = default;

    Token get();
    Token peek();
    void putback(const Token& t);
    void putback(Token&& t);
    void ignore(const Token& t) noexcept;
    void ignore(const Kind) noexcept;

    explicit operator bool() const noexcept
    {
        return source_.good() && !source_.fail();
    }

private:
    source_t& source_;
    bool      full_;
    Token     buffer_;
};