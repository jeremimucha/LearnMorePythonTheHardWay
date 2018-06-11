#pragma once

#include <string>
#include <type_traits>


class Translator {
public:
    template<typename T, typename U>
      requires std::is_convertible_v<T,std::string> && std::is_convertible_v<U,std::string>
    Translator(T&& str1, U&& str2)
        : set1_{std::forward<T>(str1)}, set2_{std::forward<U>(str2)} { }

    void truncate() noexcept
    {
        set1_.resize(set2_.size());
    }

    char translate(char ch) const noexcept
    {
        const auto idx = set1_.find_first_of(ch);
        if( idx != std::string::npos )
            return idx < set2_.size() ? set2_[idx] : set2_.back();
        else
            return ch;
    }
private:
    std::string set1_;
    std::string set2_;
};