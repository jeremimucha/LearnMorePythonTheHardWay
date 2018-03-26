#ifndef THREADSAFESTACK_GUARD_H
#define THREADSAFESTACK_GUARD_H

#include <memory>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif


template<typename> class ThreadsafeStack;
template<typename> class ThreadsafeStackNode;

struct Invariant_violation_exception : public std::logic_error
{ using std::logic_error::logic_error; };

template<typename T>
inline bool assert_invariant( const ThreadsafeStack<T>& );

/* ThreadsafeStack */
/* ------------------------------------------------------------------------- */
template<typename T>
class ThreadsafeStack
{
    template<typename U>
    friend inline bool assert_invariant( const ThreadsafeStack<U>& );
    struct node;
    using Node_allocator = std::allocator<node>;
    using T_allocator = std::allocator<T>;
    using Node_alloc_traits = std::allocator_traits<Node_allocator>;
    using T_alloc_traits = std::allocator_traits<T_allocator>;
    using mutex_type = std::mutex;
    using condition_variable_type = std::condition_variable;
    using self = ThreadsafeStack;

    struct node{
        node() noexcept = default;
        template<typename... Args>
        explicit node( Args&&... args )
            : data{ std::make_unique<T>(std::forward<Args>(args)...) } { }

    // --- each node owns it's data
        std::unique_ptr<T>    data;
        std::unique_ptr<node> next{nullptr};
    };

public:
    // using allocator_type = T_allocator;
    using value_type            = T;
    using reference             = value_type&;
    using const_reference       = const value_type&;
    using pointer               = T*;
    using const_pointer         = const T*;
    using difference_type       = std::ptrdiff_t;
    using size_type             = difference_type; // intentionally a signed type

// --- No iterators by design

// --- constructors - only the default constructor by design
    ThreadsafeStack() noexcept = default;

// --- destructor
    ~ThreadsafeStack() noexcept = default;

// --- copy control
    ThreadsafeStack( const ThreadsafeStack& ) = delete;
    ThreadsafeStack( ThreadsafeStack&& ) noexcept = delete;
    ThreadsafeStack& operator=( const ThreadsafeStack& ) = delete;
    ThreadsafeStack& operator=( ThreadsafeStack&& ) noexcept = delete;

// --- capacity
    bool empty() const noexcept { return top == nullptr; }

// --- modifiers
    inline void push( const T& value );

    inline void push( T&& value ) noexcept;

    template<typename... Args>
    inline void emplace( Args&&... args );

    inline bool try_pop( T& value );

    inline std::unique_ptr<T> try_pop() noexcept;

    inline void wait_and_pop( T& value ) noexcept;

    inline std::unique_ptr<T> wait_and_pop() noexcept;

    template<typename> friend inline
    void swap( ThreadsafeStack<T>& lhs, ThreadsafeStack<T>& rhs ) noexcept;

protected:
    template<typename... Args> inline
    std::unique_ptr<node> make_node( Args&&... args);

    inline std::unique_ptr<node> unlink_top() noexcept;

private:
// --- member data
    mutable mutex_type              top_mutex{};
    mutable condition_variable_type top_cond{};
    std::unique_ptr<node>           top;
};
/* ------------------------------------------------------------------------- */


// --- implementation
#include "ThreadsafeStack.impl.h"


#endif /* THREADSAFESTACK_GUARD_H */
