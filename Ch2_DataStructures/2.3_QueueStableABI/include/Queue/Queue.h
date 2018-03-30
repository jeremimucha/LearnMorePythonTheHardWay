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

bool assert_invariant( const QueueBase& );
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
    explicit QueueNode( QueueNodeBase* next_, Args&&... args )
        : QueueNodeBase{next_}, data{std::forward<Args>(args)...}
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

    QueueBase() noexcept : head_{&head_}, tail_{&head_} { }

    QueueBase( node_base* first, node_base* last ) noexcept
        : head_{first}, tail_{last}
        {
            tail_->next = &head_;
        }

    bool empty() const noexcept { return tail_ == &head_; }

    void push_back_( node_base* nn ) noexcept
    { Expects(nn->next == &head_);
        tail_->next = nn;
        tail_ = nn;
    }

    node_base* get_front_() noexcept
        { return head_.next; }

    const node_base* get_front_() const noexcept
        { return head_.next; }

    node_base* get_back_() noexcept
        { return tail_; }

    const node_base* get_back_() const noexcept
        { return tail_; }

    node_base* unlink_head_() noexcept
    {
        auto* const target = head_.next;
        head_.next = target->next;
        if( head_.next == &head_)
            tail_ = &head_;
        return target;
    }

    node_base  head_;
    node_base* tail_;
};

// Empty queue -> tail_ points at head_, head_ points at nullptr. This makes pushing
// simpler. tail_ == &head && head == nullptr
// Single element -> head.next and tail point at the same element,
// tail_ will need to be set to &head on last element's removal.
// Or every inserted element's next pointer could be set to &head on construction?

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
    ~Queue() noexcept { free(); }

// --- copy control
    Queue( const Queue& );
    Queue( Queue&& ) noexcept;
    Queue& operator=(const Queue& ) = delete;
    Queue& operator=( Queue&& ) noexcept = delete;

// --- copy control
    using base::empty;

// --- element access
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

// --- modifiers
    void push( const T& );
    void push( T&& );
    
    template<typename... Args>
    decltype(auto) emplace( Args&&... args );

    void pop();

    template<typename U>
    inline void swap( Queue<U>& lhs, Queue<U>& rhs ) noexcept;

protected:
    template<typename... Args>
    inline node* make_node( Args&&... args );
    inline std::pair<node*,node*> alloc_range(const node_base*, const node_base*);
    inline void free( not_null<node*> ) noexcept;
    inline void free( node*, node* ) noexcept;
    inline void free() noexcept;

private:
    Allocator alloc{Allocator{}};
};

// --- implementation
#include "Queue.impl.h"
/* ------------------------------------------------------------------------- */

#endif /* QUEUE_GUARD_H_ */
