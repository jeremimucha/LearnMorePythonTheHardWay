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

/**
 * Non owning SuffixArray. Uses std::string_view to find suffixes in a refered to string
 */

class SuffixArray
{

public:
    using suffix_t        = std::string_view;
    using size_type       = typename suffix_t::size_type;
    using value_type      = std::pair<suffix_t, size_type>;
    using suffix_vector_t = std::vector<value_type>;
    using iterator        = typename suffix_vector_t::iterator;
    using const_iterator  = typename suffix_vector_t::const_iterator;

    explicit SuffixArray( suffix_t str )
        : data{std::move(str)}
        {
            suffixes.reserve(data.size());
            for( size_type i=0; i<data.size(); ++i ){
                suffixes.emplace_back(&data[i], data.size()-i);
            }
            auto pred = [](value_type lhs, value_type rhs){
                return lhs.first < rhs.first;
            };
            std::sort(suffixes.begin(), suffixes.end(), pred);
        }

// --- iterators
    iterator begin() { return suffixes.begin(); }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return suffixes.cbegin(); }
    iterator end() { return suffixes.end(); }
    const_iterator end() const { return cend(); }
    const_iterator cend() const { return suffixes.cend(); }

    iterator find(suffix_t substr) noexcept
    {
        return find_impl(*this, substr);
    }

    const_iterator find(suffix_t substr) const noexcept
    {
        return find_impl(*this, substr);
    }

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

    std::vector<value_type> find_all( suffix_t substr ) const
    {
        auto result = std::vector<value_type>{};
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

        bool operator()(value_type s) const
        {
            return s.first.size() >= substr.size()
                && substr == s.first.substr(0,substr.size());
        }

        const suffix_t substr;
    };

    template<typename T>
    static auto find_impl(T& obj, suffix_t substr)
        -> decltype(obj.find(substr))
    {
        auto pred = [](value_type elem, auto val){
            return elem.first < val;
        };
        auto res = std::lower_bound(obj.suffixes.begin(), obj.suffixes.end(), substr, pred);
        return res != obj.suffixes.end() && res->first == substr ? res : obj.suffixes.end();
    }

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
            if(it != obj.suffixes.end() && it->first.size() < res->first.size())
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
            if(it != obj.suffixes.end() && res->first.size() < it->first.size())
                res = it;
        }

        return result_type{res};
    }

friend std::ostream& operator<<(std::ostream& os, const SuffixArray& s);

private:
    suffix_t        data;
    suffix_vector_t suffixes;
};

#endif /* SUFFIX_ARRAY_GUARD_H_ */
