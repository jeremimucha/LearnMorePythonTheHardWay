#ifndef sort_HEADER_INCLUDED_HPP_
#define sort_HEADER_INCLUDED_HPP_

#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <gsl/gsl>


namespace jam
{


/* Reading a stream line by line */
/* ------------------------------------------------------------------------- */
template<typename Container>
auto get_lines( std::istream& is ) -> Container
    // requires push_back()
{
    auto lines = Container{};
    for( std::string line; getline(is, line); ){
        lines.emplace_back(std::move(line));
    }
    return lines;
}

template<typename Container>
void get_lines( std::istream& is, Container& container )
{
    for( std::string line; getline(is, line); ){
        container.emplace_back( std::move(line) );
    }
}

// --- Filter based on Predicate

template<typename Container, typename Predicate>
auto get_lines( std::istream& is, Predicate predicate ) -> Container
{
    auto lines = Container{};
    for( std::string line; getline(is, line); ){
        if( predicate(line) ){
            lines.emplace_back( std::move(line) );
        }
    }
    return lines;
}

template<typename Container, typename Predicate>
void get_lines( std::istream& is, Container& container, Predicate predicate )
{
    for( std::string line; getline(is, line); ){
        if( predicate(line) ){
            container.emplace_back( std::move(line) );
        }
    }
}
/* ------------------------------------------------------------------------- */

/* Sorting */
/* ------------------------------------------------------------------------- */
template<typename Container, typename Predicate>
void sort_lines(Container& lines, Predicate pred)
{
    std::sort(lines.begin(), lines.end(), pred);
}

template<typename T, typename Predicate>
void sort_lines( std::list<T>& lines, Predicate pred )
{
    lines.sort(pred);
}


/* Composible predicates */
/* ------------------------------------------------------------------------- */
template<typename Derived>
class PredicateBase
{
public:
    Derived* underlying() { return static_cast<Derived*>(this); }
    const Derived* underlying() const { return static_cast<const Derived*>(this); }
    
    template<typename T>
    auto operator()( T&& t ) -> bool
    {
        return underlying()->operator()(std::forward<T>(t));
    }

    template<typename T>
    auto operator()( T&& t ) const -> bool
    {
        return underlying()->operator()(std::forward<T>(t));
    }

    template<typename T>
    auto operator()( T&& lhs, T&& rhs ) -> bool
    {
        return underlying()->operator()( std::forward<T>(lhs), std::forward<T>(rhs) );
    }

    template<typename T>
    auto operator()( T&& lhs, T&& rhs ) const -> bool
    {
        return underlying()->operator()( std::forward<T>(lhs), std::forward<T>(rhs) );
    }
private:
    PredicateBase() noexcept = default;
    friend Derived;
};


template<typename Predicate, typename Function>
class PredicateWrapper : public PredicateBase<PredicateWrapper<Predicate,Function>>
{
public:
    constexpr PredicateWrapper( Predicate&& predicate, Function&& function )
        : m_predicate(std::forward<Predicate>(predicate))
        , m_function(std::forward<Function>(function))
        { }
        
        template<typename T>
        auto operator()( T&& t ) -> bool
        {
            return m_predicate( m_function(std::forward<T>(t)) );
        }
private:
    Predicate m_predicate;
    Function m_function;
};

template<typename Predicate, typename Function>
auto wrap_predicate( Predicate&& predicate, Function function )
{
    return PredicateWrapper<Predicate,Function>
                            (std::forward<Predicate>(predicate),
                             std::forward<Function>(function)
                            );
}

template<typename Predicate, typename Function, typename... Functions>
auto wrap_predicate( Predicate&& predicate, Function&& function, Functions&&... functions )
{
    return wrap_predicate(
            PredicateWrapper<Predicate,Function>(
                 std::forward<Predicate>(predicate)
                ,std::forward<Function>(function))
            ,std::forward<Functions>(functions)...
            );
}

template<typename WPred, typename WFunc, typename Function>
auto operator&&( PredicateWrapper<WPred,WFunc>&& predicate,
                 Function function )
{
    return PredicateWrapper<PredicateWrapper<WPred,WFunc>,Function>
                            (std::forward<PredicateWrapper<WPred,WFunc>>(predicate)
                            ,std::forward<Function>(function)
                            );
}


// --- Binary Predicates
template<typename Predicate, typename Function>
class BinaryPredicateWrapper : public PredicateBase<BinaryPredicateWrapper<Predicate,Function>>
{
public:
    constexpr BinaryPredicateWrapper( Predicate&& predicate, Function&& function )
        : m_predicate(std::forward<Predicate>(predicate))
        , m_function(std::forward<Function>(function))
        { }
        
        template<typename T>
        auto operator()( T&& lhs, T&& rhs ) -> bool
        {
            return m_predicate( m_function(std::forward<T>(lhs)),
                                m_function(std::forward<T>(rhs))
                              );
        }
private:
    Predicate m_predicate;
    Function m_function;
};

template<typename Predicate, typename Function>
auto wrap_binary_predicate( Predicate&& predicate, Function function )
{
    return BinaryPredicateWrapper<Predicate,Function>
                                  (std::forward<Predicate>(predicate),
                                   std::forward<Function>(function)
                                  );
}

template<typename Predicate, typename Function, typename... Functions>
auto wrap_binary_predicate( Predicate&& predicate, Function function, Functions&&... functions )
{
    return wrap_binary_predicate(
                BinaryPredicateWrapper<Predicate,Function>(
                    std::forward<Predicate>(predicate)
                   ,std::forward<Function>(function))
            ,std::forward<Functions>(functions)...
            );
}

template<typename WPred, typename WFunc, typename Function>
auto operator&&( BinaryPredicateWrapper<WPred,WFunc>&& predicate,
                 Function function )
{
    return BinaryPredicateWrapper<BinaryPredicateWrapper<WPred,WFunc>,Function>
                (std::forward<BinaryPredicateWrapper<WPred,WFunc>>(predicate)
                ,std::forward<Function>(function)
                );
}

template<typename Predicate, typename Function>
using Pred = PredicateBase<PredicateWrapper<Predicate,Function>>;

template<typename Predicate, typename Function>
using BiPred = PredicateBase<BinaryPredicateWrapper<Predicate,Function>>;
/* ------------------------------------------------------------------------- */


} // namespace


#endif /* sort_HEADER_INCLUDED_HPP_ */

