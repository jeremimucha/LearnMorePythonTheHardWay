#pragma once

#include <istream>
#include <string>
#include <utility>
#include <stdexcept>

#include "Keywords.h"


enum class Kind
{
    Cmd, Quit, Terminator, Pipe, EscapeSlash
};

class Bad_kind_exception : public std::logic_error {
public:
    explicit Bad_kind_exception(const char ch)
        : std::logic_error{"Unknown Kind character " + std::to_string(ch)} { }
};

class Token
{
public:
    friend constexpr Kind operator "" _Kind(const char ch);

    Token(Kind kind)
        : kind_{kind}
        { }
    Token(std::string val)
        : kind_{Kind::Cmd}, value_{std::move(val)}
        { }
    Token(const char ch)
        : kind_{make_kind(ch)}, value_{ch}
        { }
    ~Token() noexcept = default;

    Token(const Token&) = default;
    Token(Token&&) = default;
    Token& operator=(const Token&) = default;
    Token& operator=(Token&&) noexcept = default;

    Kind kind() const noexcept { return kind_; }
    const std::string& value() const noexcept { return value_; }

friend bool operator==(const Token& lhs, const Token& rhs) noexcept
{
    return lhs.kind_ == rhs.kind_ && lhs.value_ == rhs.value_;
}
friend bool operator!=(const Token& lhs, const Token& rhs) noexcept
{
    return !(lhs == rhs);
}

protected:
    static constexpr Kind make_kind(const char ch)
    {
        switch(ch){
            case Colon:
            case Eol:
            case Cr:
                return Kind::Terminator;
            case EscapeSlash:
                return Kind::EscapeSlash;
            case Pipe:
                return Kind::Pipe;
            default:
                throw Bad_kind_exception{ch};
        }
    }

private:
    Kind        kind_;
    std::string value_{};
};

constexpr Kind operator "" _Kind(const char ch)
{
    return Token::make_kind(ch);
}

inline bool is_operator(const char ch) noexcept
{
    return ch == Colon || ch == EscapeSlash || ch == Eol || ch == Pipe;
}
