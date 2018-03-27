#ifndef STACK_STABLEABI_GUARD_H_
#define STACK_STABLEABI_GUARD_H_

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

template<typename> class Stack;
template<typename> class StackNode;

struct Invariant_violation_exception : public std::logic_error
{ using std::logic_error::logic_error; };


/* StackNode */
/* ------------------------------------------------------------------------- */
struct StackNodeBase
{
    StackNodeBase* next{nullptr};
};

template<typename T>
struct StackNode : public StackNodeBase
{
    template<typename... Args>
    explicit StackNode( Args&&... args )
        : StackNodeBase{}, m_data{std::forward<Args>(args)...}
        { }

    T m_data;
};
/* ------------------------------------------------------------------------- */

/* Stack */
/* ------------------------------------------------------------------------- */
class StackBase
{
    friend bool assert_invariant( const StackBase& );
protected:
    using node_base = StackNodeBase;

    inline void link_nodes( node_base* pred, node_base* succ ) noexcept
    {
        pred->next = succ;
    }

    inline void push_top( node_base* n ) noexcept
    {
        n->next = m_top;
        m_top = n;
    }

    inline node_base* unlink_top() noexcept
    {
        auto target = m_top;
        m_top = target->next;
        return target;
    }

    node_base* m_top{nullptr};
};

bool assert_invariant( const StackBase& obj );


template<typename T>
class Stack : public StackBase
{
    using node = StackNode<T>;
    using Allocator = std::allocator<node>;
    using T_allocator = std::allocator<T>;
    using alloc_traits = std::allocator_traits<Allocator>;
    using T_alloc_traits = std::allocator_traits<T_allocator>;
    using base = StackBase;
    using self = Stack;
public:
    using allocator_type = T_allocator;
    using value_type = typename T_alloc_traits::value_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename T_alloc_traits::pointer;
    using const_pointer = typename T_alloc_traits::const_pointer;
    using difference_type = typename T_alloc_traits::difference_type;
    using size_type = difference_type;
// --- No iterators by design

// --- constructors - only the default
    Stack() noexcept = default;

// --- destructor
    ~Stack() noexcept { free(); }

// --- copy control
    Stack( const Stack& ) = delete;
    Stack( Stack&& ) noexcept = delete;
    Stack& operator=( const Stack& ) = delete;
    Stack& operator=( Stack&& ) = delete;

// --- element access
    reference top()
    { Expects(m_top != nullptr);
        return static_cast<node*>(m_top)->m_data;
    }

    const_reference top() const
    { Expects(m_top != nullptr);
        return static_cast<const node*>(m_top)->m_data;
    }

// --- capacity
    bool empty() const noexcept;

// --- modifiers
    void push( const T& value );
    void push( T&& value );

    template<typename... Args>
    decltype(auto) emplace( Args&&... args );

    void pop();

    template<typename>
    friend inline void swap( Stack<T>& lhs, Stack<T>& rhs ) noexcept;

protected:
    template<typename... Args>
    inline node* make_node( Args&&... args );

    inline void free( not_null<node*> target ) noexcept;

    inline void free( node* first, node* last ) noexcept;

    inline void free() noexcept;

private:
    Allocator alloc{Allocator{}};
};
/* ------------------------------------------------------------------------- */



// --- implementation
#include "Stack.impl.h"


#endif /* STACK_STABLEABI_GUARD_H_ */
