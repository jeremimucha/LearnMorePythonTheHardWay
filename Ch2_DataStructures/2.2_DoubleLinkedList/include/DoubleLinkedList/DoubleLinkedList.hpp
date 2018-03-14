#ifndef DOUBLELINKEDLIST_HPP_
#define DOUBLELINKEDLIST_HPP_

#include <memory>
#include <utility>
#include <algorithm>
#include <iterator>
#include <initializer_list>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif

using gsl::not_null;
using gsl::owner;


template<typename> class Node;
template<typename> class DoubleLinkedList;
template<typename> class DLL_iterator;
template<typename> class DLL_const_iterator;


/* DLL Node<T> */
/* ------------------------------------------------------------------------- */
template<typename T>
struct Node
{
friend class DoubleLinkedList<T>;
friend class DLL_iterator<T>;
friend class DLL_const_iterator<T>;

    Node() noexcept = default;
    template<typename... Args>
    explicit Node( Args&&... args )
        : data{std::forward<Args>(args)...}
        { }

    T       data;
    Node*   next{nullptr};
    Node*   prev{nullptr};
};
/* ------------------------------------------------------------------------- */

class InvariantViolationException : public std::logic_error
{
    using std::logic_error::logic_error;
};

template<typename T>
void ensure_invariant_empty( const DoubleLinkedList<T>& object )
{
    // invariant for an empty list
    
}

template<typename T>
void ensure_invariant( const DoubleLinkedList<T>& object )
{
    if( object.head.prev != nullptr )
        throw InvariantViolationException("head.prev != nullptr");
    if( object.tail.next != nullptr )
        throw InvariantViolationException("tail.next != nullptr");
    if( object.head.next == &object.head.tail )
        ensure_invariant_empty(object);
    else
        ensure_invariant_nonempty(object);
}

/* DoubleLinkedList */
/* ------------------------------------------------------------------------- */
template<typename T>
class DoubleLinkedList
{
    friend invariant<T>( const DoubleLinkedList<T>& );
    using node                      = Node<T>;
    using Allocator                 = std::allocator<node>;
    using T_Allocator               = std::allocator<T>;
    using alloc_traits              = std::allocator_traits<Allocator>;
    using T_alloc_traits            = std::allocator_traits<T_Allocator>;
    using self                      = DoubleLinkedList;
public:
    using allocator_type            = T_Allocator;
    using value_type                = typename T_alloc_traits::value_type;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using pointer                   = typename T_alloc_traits::pointer;
    using const_pointer             = typename T_alloc_traits::const_pointer;
    using difference_type           = typename T_alloc_traits::difference_type;
    using size_type                 = typename T_alloc_traits::size_type;

    using iterator                  = DLL_iterator<T>;
    using const_iterator            = DLL_const_iterator<T>;
    using reverse_iterator          = std::reverse_iterator<iterator>;
    using const_reverse_iterator    = std::reverse_iterator<const_iterator>;


// --- constructors
    DoubleLinkedList() noexcept = default;

    explicit DoubleLinkedList( size_type count );
    DoubleLinkedList( size_type count, const T& value );
    DoubleLinkedList( std::initializer_list<T> ilist );

// --- destructor
    ~DoubleLinkedList() noexcept;

// --- copy control
    DoubleLinkedList( const DoubleLinkedList& other );
    DoubleLinkedList( DoubleLinkedList&& other ) noexcept;
    DoubleLinkedList& operator=( const DoubleLinkedList& other );
    DoubleLinkedList& operator=( DoubleLinkedList&& other );
    DoubleLinkedList& operator( initializer_list<T> ilist );

// --- element access
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

// --- iterators
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

// --- capacity
    bool empty() const noexcept;
    size_type size() const noexcept;

// --- modifiers
    void clear() noexcept;
    iterator insert( const_iterator pos, const T& value );
    iterator insert( const_iterator pos, T&& value );
    iterator insert( const_iterator pos, const T& value );
    template<typename InputIt>
    iterator insert( const_iterator pos, InputIt first, InputIt last );
    iterator insert( const_iterator pos, std::initializer_list<T> ilist );
    template<typename... Args>
    iterator emplace( const_iterator pos, Args&&... args );

    iterator erase( const_iterator pos );
    iterator erase( const_iterator first, const_iterator last );

    void push_back( const T& value );
    void push_back( T&& value );
    template<typename... Args>
    reference emplace_back( Args&&... args );
    void pop_back();

    void push_front( const T& value );
    void push_front( T&& value );
    template<typename... Args>
    reference emplace_front( Args&&... args );
    void pop_front();

    void resize( size_type count );
    void resize( size_type count, const T& value );

    void swap( DoubleLinkedList& other ) noexcept;

    friend void swap( DoubleLinkedList& lhs, DoubleLinkedList& rhs ) noexcept;

protected:
    template<typename... Args>
    inline node* make_node( Args&&... args )
    {
        auto nn = alloc.allocate(1);
        alloc.construct(nn, std::forward<Args>(args)...);

        Ensures(nn != nullptr);
        return nn;
    }

    inline void link_nodes( not_null<node*> pred, not_null<node*> succ )
    {
        pred->next = succ;
        succ->prev = pred;
    }

    inline void link_nodes( not_null<node*> pred
                   , std::pair<node*,node*> range
                   , not_null<node*> succ )
    {
        // insert a range of linked nodes beginning with 'range.first'
        // and ending with 'range.second' inbetween nodes 'pred' and 'succ'
        range.first->prev = pred;
        range.second->next = succ;
        pred->next = range.first;
        succ->prev = range.second;
    }

    template<typename... Args>
    std::pair<node*,node*> alloc_n( size_type n, Args&&... args )
    { Expects( n != 0 );
        auto first = make_node(std::forward<Args>(args)...);
        auto pred = first;
        while( --n ){
            auto succ = make_node(std::forward<Args>(args)...);
            link_nodes(pred, succ);
            pred = succ;
        }
        return {first, pred};
    }

    template<typename InputIt>
    std::pair<node*,node*> alloc_range( InputIt first, InputIt last )
    { Expects(first != nullptr && last != nullptr);
        auto first = make_node(*first);
        auto pred = first;
        while( ++first != last ){
            auto succ = make_node(*first);
            link_nodes(pred, succ);
            pred = succ;
        }
        return {first, pred};
    }

    void free() noexcept
    {
        while( head.next != &tail ){
            auto temp = head.next;
            head.next = temp->next;
            alloc.destroy(temp);
            alloc.deallocate(temp, 1);
        }
        tail.prev = &head;
    }
private:
    Allocator alloc{Allocator()};
    node head{};
    node tail{};
};
/* ------------------------------------------------------------------------- */


/* DLL_iterator<T> */
/* ------------------------------------------------------------------------- */
template<typename T>
class DLL_iterator
{
    friend class DoubleLinkedList<T>;
    using Container                     = DoubleLinkedList<T>;
    using node                          = Node<T>;
    using self                          = DLL_iterator;
public:
    using value_type                    = typename Container::value_type;
    using reference                     = typename Container::reference;
    using pointer                       = typename Container::pointer;
    using iterator_category             = std::bidirectional_iterator_tag;
    using difference_type               = typename Container::difference_type;
    using const_iterator                = DLL_const_iterator<T>;

    explicit DLL_iterator( node* n ) noexcept
        : base{n} { }

    reference operator*() const noexcept
        { return base->data; }

    pointer operator->() const noexcept
        { return &(base->data); }

    self& operator++() noexcept
        {
            base = base->next;
            return *this;
        }
    
    self operator++(int) noexcept
        {
            auto rv = *this;
            base = base->next;
            return rv;
        }

    self& operator--() noexcept
        {
            base = base->prev;
            return *this;
        }

    self operator--(int) noexcept
        {
            auto rv = *this;
            base = base->prev;
            return *this;
        }

    operator const_iterator() { return const_iterator(base); }
private:
    node* base;
};
/* ------------------------------------------------------------------------- */

/* DLL_const_iterator<T> */
/* ------------------------------------------------------------------------- */
template<typename T>
class DLL_const_iterator
{
    friend class DoubleLinkedList<T>;
    using Container                     = DoubleLinkedList<T>;
    using node                          = Node<T>;
    using self                          = DLL_const_iterator;
public:
    using value_type                    = typename Container::value_type;
    using reference                     = typename Container::const_reference;
    using pointer                       = typename Container::const_pointer;
    using iterator_category             = std::bidirectional_iterator_tag;
    using difference_type               = typename Container::difference_type;

    explicit DLL_const_iterator( const node* n ) noexcept
        : base{n} { }

    reference operator*() const noexcept
        { return base->data; }

    pointer operator->() const noexcept
        { return &(base->data); }

    self& operator++() noexcept
        {
            base = base->next;
            return *this;
        }
    
    self operator++(int) noexcept
        {
            auto rv = *this;
            base = base->next;
            return rv;
        }

    self& operator--() noexcept
        {
            base = base->prev;
            return *this;
        }

    self operator--(int) noexcept
        {
            auto rv = *this;
            base = base->prev;
            return *this;
        }
private:
    const node* base;
};
/* ------------------------------------------------------------------------- */
#endif /* DOUBLELINKEDLIST_HPP_ */
