#ifndef STACK_CONCURRENT_GUARD_HPP_
#define STACK_CONCURRENT_GUARD_HPP_

#include <memory>
#include <utility>
#include <initializer_list>
#include <thread>
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

using gsl::not_null;


template<typename> class ThreadsafeStack;
template<typename> class ThreadsafeStackNode;

struct Invariant_violation_exception : public std::logic_error
{ using std::logic_error::logic_error; };

template<typename T>
inline bool assert_invariant( const ThreadsafeStack<T>& );


/* ThreadsafeStackNode */
/* ------------------------------------------------------------------------- */
template<typename T>
struct ThreadsafeStackNode
{
    ThreadsafeStackNode() noexcept = default;
    
    template<typename... Args>
    explicit ThreadsafeStackNode( Args&&... args )
        : data{std::forward<Args>(args)...}
        { }

    T                    data;
    ThreadsafeStackNode* next;
};
/* ------------------------------------------------------------------------- */

/* ThreadsafeStack */
/* ------------------------------------------------------------------------- */
template<typename T>
class ThreadsafeStack
{
    template<typename U>
    friend bool assert_invariant( const ThreadsafeStack<U>& );
    using node = ThreadsafeStackNode<T>;
    using Allocator = std::allocator<node>;
    using T_Allocator = std::allocator<T>;
    using alloc_traits = std::allocator_traits<Allocator>;
    using T_alloc_traits = std::allocator_traits<T_Allocator>;
    using mutex_type = std::mutex;
    using condition_variable_type = std::condition_variable;
    using self = ThreadsafeStack;
public:
    using allocator_type = T_Allocator;
    using value_type = typename T_alloc_traits::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename T_alloc_traits::pointer;
    using const_pointer = typename T_alloc_traits::const_pointer;
    using difference_type = typename T_alloc_traits::difference_type;
    using size_type = typename T_alloc_traits::difference_type;

// --- No iterators by design
    // using iterator = ThreadsafeStack_iterator<T>;
    // using const_iterator = ThreadsafeStack_const_iterator<T>;
    // using reverse_iterator = std::reverse_iterator<iterator>;
    // using const_reverse_iterator = std::reverse_iterator<const_iterator>;

// --- constructors
    ThreadsafeStack() noexcept = default;
// --- No other than default constructor by design
    // explicit ThreadsafeStack( size_type count )
    //     : ThreadsafeStack(count, T{}) { }
    
    // ThreadsafeStack( size_type count, const T& value );

// --- destructor
    ~ThreadsafeStack() noexcept { free(); }

// --- copy control
    ThreadsafeStack( const ThreadsafeStack& ) = delete;
    ThreadsafeStack( ThreadsafeStack&& ) noexcept = delete;
    ThreadsafeStack& operator=( const ThreadsafeStack& ) = delete;
    ThreadsafeStack& operator=( ThreadsafeStack&& ) noexcept = delete;

// --- element access
    // No element access methods returning references are provided. Doing so
    // would expose the data beyond the control of the mutex.

// --- capacity
    bool empty() const noexcept { return top.next == nullptr; }

// --- modifiers
    void push( const T& value );
    
    void push( T&& value ) noexcept;
    
    template<typename... Args>
    void emplace( Args&&... args );

    bool try_pop( T& value ) noexcept;
    value_type wait_and_pop() noexcept;
    void wait_and_pop( T& value ) noexcept;

    template<typename> friend inline 
    void swap( ThreadsafeStack<T>& lhs, ThreadsafeStack<T>& rhs ) noexcept;

protected:
    template<typename... Args>
    inline node* make_node( Args&&... args );

    inline void link_nodes( not_null<node*> pred, node* succ ) noexcept;

    inline node* unlink_top() noexcept;

    inline void free() noexcept;
    inline void free( not_null<node*> ) noexcept;
    inline void free( node*, node* ) noexcept;

private:
    Allocator                       alloc{ Allocator{} };
    mutable mutex_type              top_mutex{};
    mutable condition_variable_type top_cond{};
    node                            top;
};
/* ------------------------------------------------------------------------- */


// --- implementation
#include "ThreadsafeStack.impl.h"


#endif /* STACK_CONCURRENT_GUARD_HPP_ */
