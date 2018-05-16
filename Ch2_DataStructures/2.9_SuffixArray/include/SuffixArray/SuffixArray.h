#ifndef SUFFIX_ARRAY_GUARD_H_
#define SUFFIX_ARRAY_GUARD_H_

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <utility>
#include <istream>
#include <ostream>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif


class SuffixArray
{
    using string_t        = std::string;

public:
    using suffix_t        = std::string_view;
    using suffix_vector_t = std::vector<suffix_t>;
    using size_type       = typename string_t::size_type;
    using iterator        = typename suffix_vector_t::iterator;
    using const_iterator  = typename suffix_vector_t::const_iterator;

    explicit SuffixArray( std::string str )
        : data{std::move(str)}
        {
            suffixes.reserve(data.size());
            for( size_type i=0; i<data.size(); ++i ){
                suffixes.emplace_back(&data[i], data.size()-i);
            }

            std::sort(suffixes.begin(), suffixes.end());
        }

// --- iterators
    iterator begin() { return suffixes.begin(); }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return suffixes.cbegin(); }
    iterator end() { return suffixes.end(); }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return suffixes.cend(); }


    iterator find_shortest( suffix_t substr) noexcept
    {
        return find_shortest_impl(*this, substr);
    }

    const_iterator find_shortest( suffix_t substr) const noexcept
    {
        return find_shortest_impl(*this, substr);
    }

    iterator find_longest( suffix_t substr ) noexcept
    {
        return find_longest_impl(*this, substr);
    }

    const_iterator find_longest( suffix_t substr ) const noexcept
    {
        return find_longest_impl(*this, substr);
    }

    suffix_vector_t find_all( suffix_t substr ) const
    {
        auto result = suffix_vector_t{};
        auto pred = IsSubstrPred{substr};
        for(const auto s : suffixes){
            if(pred(s)) result.push_back(s);
        }
        return result;
    }

protected:
    struct IsSubstrPred {
        explicit IsSubstrPred(suffix_t substr_)
            : substr{std::move(substr_)} { }

        bool operator()(suffix_t s) const
        {
            return s.size() >= substr.size() && substr == s.substr(0,substr.size());
        }

        const suffix_t substr;
    };

    template<typename T>
    static auto find_shortest_impl( T& obj, suffix_t substr)
    -> decltype(obj.find_shortest(substr))
    {
        using result_type = decltype(obj.find_shortest(substr));

        auto pred = IsSubstrPred{substr};
        auto res = std::find_if(obj.suffixes.begin(), obj.suffixes.end(), pred);
        for(auto it=res; it != obj.suffixes.end();){
            ++it;
            it = std::find_if(it, obj.suffixes.end(), pred);
            if(it != obj.suffixes.end() && it->size() < res->size())
                res = it;
        }

        return result_type{res}; 
    }

    template<typename T>
    static auto find_longest_impl( T& obj, suffix_t substr )
    -> decltype(obj.find_longest(substr))
    {
        using result_type = decltype(obj.find_longest(substr));

        auto pred = IsSubstrPred{substr};
        auto res = std::find_if(obj.suffixes.begin(), obj.suffixes.end(), pred);
        for(auto it=res; it != obj.suffixes.end();){
            ++it;
            it = std::find_if(it, obj.suffixes.end(), pred);
            if(it != obj.suffixes.end() && res->size() < it->size())
                res = it;
        }

        return result_type{res};
    }

friend std::ostream& operator<<(std::ostream& os, const SuffixArray& s);

private:
    string_t        data;
    suffix_vector_t suffixes;
};

#endif /* SUFFIX_ARRAY_GUARD_H_ */
