#ifndef THREADSAFE_QUEUE_H_
#define THREADSAFE_QUEUE_H_

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

using gsl::not_null;


template<typename> class ThreadsafeQueue;

template<typename T>
inline bool assert_invariant( const ThreadsafeQueue<T>& );

struct Invariant_violation_exception : public std::logic_error
{ using std::logic_error::logic_error; };


/* ThreadsafeQueue */
/* ------------------------------------------------------------------------- */
template<typename T>
class ThreadsafeQueue
{
    template<typename U>
    friend bool assert_invariant( const ThreadsafeQueue<U>& );

    using mutex_type              = std::mutex;
    using condition_variable_type = std::condition_variable;
    using self                    = ThreadsafeQueue;

    struct node{
        std::unique_ptr<node>  next;
        std::shared_ptr<T>     data;
    };

public:
    using value_type        = T;
    using reference         = value_type&;
    using const_reference   = const value_type&;
    using pointer           = T*;
    using const_pointer     = const T*;
    using difference_type   = std::ptrdiff_t;
    using size_type         = difference_type;

// --- constructors - only the default
    ThreadsafeQueue()
        : head( new node ), tail(head.get())
        { }

// --- destructor
    ~ThreadsafeQueue() noexcept = default;

// --- copy control
    ThreadsafeQueue( const self& ) = delete;
    ThreadsafeQueue( self&& ) noexcept = delete;
    ThreadsafeQueue& operator=( const self& ) = delete;
    ThreadsafeQueue& operator=( self&& ) noexcept = delete;

// --- capacity
    inline bool empty() const
    { std::lock_guard<mutex_type> head_lk{head_mutex};
        return head.get() == get_tail();
    }

// --- modifiers
    void push( const T& value );
    void push( T&& value );
    template<typename... Args>
    void emplace( Args&&... args );
    std::shared_ptr<T> try_pop();
    bool try_pop( T& value );
    std::shared_ptr<T> wait_and_pop();
    void wait_and_pop( T& value );


protected:
    inline node* get_tail() const;
    std::unique_lock<mutex_type> wait_for_data();
    std::unique_ptr<node> pop_head();
    std::unique_ptr<node> wait_pop_head();
    std::unique_ptr<node> wait_pop_head(T& value);
    std::unique_ptr<node> try_pop_head();
    std::unique_ptr<node> try_pop_head(T& value);

private:
    mutable mutex_type              head_mutex;
    std::unique_ptr<node>           head;
    mutable mutex_type              tail_mutex;
    node*                           tail;
    mutable condition_variable_type cond_var;
};

// implementation
#include "ThreadsafeQueue.impl.h"

/* ------------------------------------------------------------------------- */

#endif /* THREADSAFE_QUEUE_H_ */
