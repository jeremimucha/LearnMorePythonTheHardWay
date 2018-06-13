#pragma once

#include <iosfwd>

#include "Token.h"

/* Grammar:
 * 
 */


class TokenStream
{
public:
    explicit TokenStream(std::istream& is)
        : source_{is}, full_{false}, buffer_{Kind::Cmd}
        { }
    ~TokenStream() noexcept = default;

    TokenStream(const TokenStream&) = delete;
    TokenStream(TokenStream&&) = delete;
    TokenStream& operator=(const TokenStream&) = delete;
    TokenStream& operator=(TokenStream&&) = delete;

    Token get();
    void putback(const Token& t);
    void putback(Token&& t);
    void ignore(const Token& t);
    void ignore(const Kind k);

    explicit operator bool() const noexcept
        { return source_.good() && !source_.fail(); }
private:
    std::istream& source_;
    bool full_;
    Token buffer_;
};
