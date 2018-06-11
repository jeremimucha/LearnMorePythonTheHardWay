#pragma once

#include <istream>
#include <string>
#include <string_view>
#include <type_traits>


class Filtering_istream {
public:
    struct control_field { unsigned char del : 1, squeeze : 1; };
    explicit Filtering_istream(std::istream& is)
        : is_{is} { }
    
    void squeeze_set(std::string_view set) noexcept { squeeze_set_ = set; }
    void del_set(std::string_view set) noexcept { del_set_ = set; }

    bool del() const noexcept { return flags_.del; }
    void del(bool state) noexcept { flags_.del = state; }
    
    bool squeeze() const noexcept { return flags_.squeeze; }
    void squeeze(bool state) noexcept { flags_.squeeze = state; }

    Filtering_istream& get(char& ch)
    {
        if( is_.get(ch) && flags_.squeeze &&
            squeeze_set_.find_first_of(ch) != std::string_view::npos ){
            while( is_ && is_.peek() == ch )
                is_.get();
        }
        if( is_ && flags_.del && del_set_.find_first_of(ch) != std::string_view::npos ){
            while( is_.get(ch) && del_set_.find_first_of(ch) != std::string_view::npos )
                ;
            is_.unget();
            return get(ch);
        }
        return *this;
    }

    explicit operator bool() const noexcept { return !is_.fail() && is_.good(); }

private:
    control_field flags_{0, 0};
    std::istream& is_;
    std::string_view squeeze_set_;
    std::string_view del_set_;
};