#include <istream>
#include <sstream>
#include <stdexcept>

#include "TokenStream.h"


Token TokenStream::get()
{
    if(full_){
        full_ = false;
        return buffer_;
    }

    char ch;
    source_ >> ch;

    switch(ch){
    case Colon:
    case Eol:
    case Pipe:
        return Token(ch);
    case Cr:
        source_.get(ch);
        if(ch != Eol) throw std::runtime_error("\\n expected after \\r");
        return Token(Kind::Terminator);
    case EscapeSlash:
        source_.get(ch); // get the next char which should be one of the 'special' characters
        [[fallthrough]];
    default:{
        std::ostringstream oss;
        oss << ch;
        while(source_.get(ch) && (ch != Colon && ch != Eol && ch != Pipe)){
            if(ch != EscapeSlash)
                oss << ch;
            else {
                source_.get(ch);
                oss << ch;
            }
        }
        source_.putback(ch);
        if( oss.str() == Quit ) return Token(Kind::Quit);
        return Token(std::move(oss.str()));
    } // default
    } // switch
    throw std::logic_error("Unhandled case in get");
}

void TokenStream::putback(const Token& t)
{
    if(full_) throw std::logic_error("putback() into a full buffer");
    buffer_ = t;
    full_ = true;
}

void TokenStream::putback(Token&& t)
{
    if(full_) throw std::logic_error("putback() into a full buffer");
    buffer_ = std::move(t);
    full_ = true;
}

void TokenStream::ignore(const Token& t)
{
    if( full_ && buffer_ == t){
        full_ = false;
        return;
    }
    full_ = false;
    // search input
    for(Token tok = get(); tok != t; tok = get())
        ;
}

void TokenStream::ignore(const Kind k)
{
    if( full_ && buffer_.kind() == k){
        full_ = false;
        return;
    }
    full_ = false;
    for(Token t = get(); t.kind() != k; t = get())
        ;
}
