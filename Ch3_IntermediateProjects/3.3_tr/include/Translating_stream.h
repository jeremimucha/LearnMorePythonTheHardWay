#pragma once

#include <iosfwd>
#include <utility>

#include "Translator.h"


class Translating_stream {
public:
    Translating_stream(std::ostream& os, Translator trans)
        : os_{os}, trans_{std::move(trans)} { }

    void truncate() noexcept { trans_.truncate(); }

friend Translating_stream& operator << (Translating_stream& tr, const char ch)
{
    tr.os_ << tr.trans_.translate(ch);
    return tr;
}

friend Translating_stream& operator << (Translating_stream& tr, const std::string& str)
{
    for( const auto ch : str ){
        tr << ch;
    }
    return tr;
}

template<typename T>
    requires std::is_convertible_v<std::remove_cv_t<T>, std::string>
friend Translating_stream& operator << (Translating_stream& tr, const T& data)
{
    for( const auto ch : data ){
        if( ch != '\0' ) tr << ch;
    }
    return tr;
}
    // explicit operator bool() const noexcept { return !os_.fail() && os_.good(); }
private:
    std::ostream& os_;
    Translator    trans_;
};
