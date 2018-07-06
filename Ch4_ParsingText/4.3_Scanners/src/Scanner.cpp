#include <utility>
#include <sstream>
#include "Scanner.h"


Token Scanner::get()
{
    if(full_){
        full_ = false;
        return std::move(buffer_);
    }

    char ch;
    source_.get(ch);

    switch(ch)
    {
    case '(':
    case ')':
    case '+':
    case '-':
    case ':':
    case ',':
        return Token{ch};
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        {
            source_.putback(ch);
            int val;
            source_ >> val;
            return Token{val};
        }
    case '\n':
        {
            auto oss = std::ostringstream{};
            while(source_.get(ch) && ch != '\n' && std::isspace(ch)){
                oss << ch;
            }
            source_.putback(ch);
            return Token{Kind::Indent, std::move(oss.str())};
        }
    default:
        {
            if(std::isalpha(ch))
            {
                auto oss = std::ostringstream{};
                oss << ch;
                while(source_.get(ch) && (std::isalpha(ch) || isdigit(ch) || ch == '_'))
                    oss << ch;
                source_.putback(ch);
                if(oss.str() == Def)
                    return Token{Kind::Def};
                return Token{std::move(oss.str())}; // Kind::Name
            }
            if(std::isspace(ch))
            {
                while(source_.peek(ch) && ch != '\n' && std::isspace(ch))
                    source_.ignore();
                return get();
            }
            throw std::runtime_error("Invalid Token");
        }
    } // switch
}

Token Scanner::peek()
{
    const auto ret = get();
    full_ = true;
    buffer_ = ret;
    return ret;
}

void Scanner::putback(const Token& t)
{
    if(full_)
        throw std::logic_error("putback into a full buffer");
    buffer_ = t;
    full_ = true;
}

void Scanner::putback(Token&& t)
{
    if(full_)
        throw std::logic_error("putback into a full buffer");
    buffer_ = std::move(t);
    full_ = true;
}

void Scanner::ignore(const Token& t) noexcept
{
    if(full_ && t == buffer_){
        full_ = false;
        return;
    }
    full_ = false;

    for(Token tok = get(); tok != t; tok = get())
        ;
}

void Scanner::ignore(const Kind k) noexcept
{
    if(full_ && buffer_.kind() == k){
        full_ = false;
        return;
    }
    full_ = false;
    for(Token t = get(); t.kind() != k; t = get())
        ;
}
