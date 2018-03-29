#ifndef QUEUE_GUARD_H_
#define QUEUE_GUARD_H_


#include <memory>
#include <utility>
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


class QueueBase;
template<typename> class Queue;
class QueueNodeBase;
template<typename> class QueueNode;

bool assert_invariant( QueueBase& );
struct Invariant_violation_exception : public std::logic_error
{ using std::logic_error::logic_error; };


/* QueueNode */
/* ------------------------------------------------------------------------- */
struct QueueNodeBase
{
    QueueNodeBase* next{nullptr};
};

template<typename T>
struct QueueNode : public QueueNodeBase
{
    template<typename... Args>
    explicit QueueNode( Args&&... args )
        : QueueNodeBase{}, data_{std::forward<Args>(args)...}
        { }

    T data;
};
/* ------------------------------------------------------------------------- */


/* Queue */
/* ------------------------------------------------------------------------- */
class QueueBase
{
    friend bool assert_invariant( const QueueBase& );
protected:
    using node_base = QueueNodeBase;

    node_base  head_;
    node_base* tail_;
};

// Empty queue -> tail_ points at head_ points at nullptr. This makes pushing
// simpler. tail_ == &head && head == nullptr
// Single element -> head.next and tail point at the same element,
// tail_ will need to be set to &head on last element's removal.

template<typename T>
class Queue : public QueueBase
{
    using node = QueueNode<T>;
    using Allocator = std::allocator<node>;
    using T_allocator = std::allocator<T>;
    using alloc_traits = std::allocator_traits<Allocator>;
    using T_alloc_traits = std::allocator_traits<T_allocator>;
    using base = QueueBase;
    using self = Queue;
public:
    using allocator_type = T_allocator;
    using value_type = typename T_alloc_traits::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename T_alloc_traits::pointer;
    using const_pointer = typename T_alloc_traits::const_pointer;
    using difference_type = typename T_alloc_traits::difference_type;
    using size_type = difference_type;
// --- no iterators

// --- constructors
    Queue() noexcept = default;

// --- destructor
    ~Queue() noexcept;

// --- copy control
    Queue( const Queue& );
    Queue( Queue&& ) noexcept;
    Queue& operator=(const Queue& );
    Queue& operator=( Queue&& ) noexcept;

// --- element access
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

// --- capacity
    bool empty() const noexcept;
    size_type size() const noexcept;

// --- modifiers
    void push( const T& );
    void push( T&& ) noexcept;
    
    template<typename... Args>
    decltype(auto) emplace( Args&&... args );

    void pop();

    template<typename U>
    inline void swap( Queue<U>& lhs, Queue<U>& rhs ) noexcept;
};
/* ------------------------------------------------------------------------- */

#endif /* QUEUE_GUARD_H_ */
