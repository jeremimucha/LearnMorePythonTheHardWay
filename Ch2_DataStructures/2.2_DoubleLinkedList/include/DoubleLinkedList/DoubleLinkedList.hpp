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
template<typename U> friend bool ensure_invariant( const DoubleLinkedList<U>& );

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


/* DoubleLinkedList */
/* ------------------------------------------------------------------------- */
template<typename T>
class DoubleLinkedList
{
    template<typename U>
    friend bool ensure_invariant( const DoubleLinkedList<U>& );
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
    DoubleLinkedList() noexcept
    {
        link_nodes(&head, &tail);
    }
    explicit DoubleLinkedList( size_type count )
        : DoubleLinkedList(count, T{}) { }
    DoubleLinkedList( size_type count, const T& value )
    {
        auto nodes = alloc_n(count, value);
        link_nodes(&head, nodes, &tail);
    }
    DoubleLinkedList( std::initializer_list<T> ilist )
    {
        auto nodes = alloc_range(ilist.begin(), ilist.end());
        link_nodes(&head, nodes, &tail);
    }

// --- destructor
    ~DoubleLinkedList() noexcept { free(); }

// --- copy control
    DoubleLinkedList( const DoubleLinkedList& other )
        : alloc{other.alloc}
    {
        auto nodes = alloc_range(other.cbegin(), other.cend());
        link_nodes(&head, nodes, &tail);
    }
    DoubleLinkedList( DoubleLinkedList&& other ) noexcept
        : alloc{std::move(other.alloc)}
    {
        link_nodes(&head, {other.head.next, other.tail.prev}, &tail);
        link_nodes(&other.head, &other.tail);
    }
    DoubleLinkedList& operator=( const DoubleLinkedList& other )
    {
    // potentially very expensive, should  we guard against self assignment
    // rather than do a self-assignment safe implementation?
        alloc = other.alloc;
        auto nodes = alloc_range(other.cbegin(), other.cend());
        free();
        link_nodes(&head, nodes, &tail);
        return *this;
    }
    DoubleLinkedList& operator=( DoubleLinkedList&& other ) noexcept
    {
        if( this == &other )
            return *this;
        alloc = std::move(other.alloc);
    // save a reference to the first element of the list
    // and unlink the last element
        auto temp_first = head.next;
        tail.prev->next = nullptr;
    // take ownership of the other list's elements
        link_nodes(&head, {other.head.next, other.tail.prev}, &tail);
    // leave the other list in a valid, empty state
        link_nodes(&other.head, &other.tail);
    // free our old elements
        free(temp_first, nullptr);

        return *this;
    }
    DoubleLinkedList& operator=( std::initializer_list<T> ilist )
    {
        auto nodes = alloc_range(ilist.begin(), ilist.end());
        free();
        link_nodes(&head, nodes, &tail);
        return *this;
    }

// --- element access
    reference front()
    { Expects(head.next != &tail);
        return head.next->data;
    }
    const_reference front() const
    { Expects(head.next != &tail);
        return head.next->data;
    }
    reference back()
    { Expects(tail.prev != &head);
        return tail.prev->data;
    }
    const_reference back() const
    { Expects(tail.prev != &head);
        return tail.prev->data;
    }

// --- iterators
    iterator begin() noexcept { return iterator{head.next}; }
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator cbegin() const noexcept { return const_iterator{head.next}; }
    iterator end() noexcept { return iterator{&tail}; }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cend() const noexcept { return const_iterator{&tail}; }
    reverse_iterator rbegin() noexcept { return reverse_iterator{begin()}; }
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{cbegin()}; }
    reverse_iterator rend() noexcept { return reverse_iterator{end()}; }
    const_reverse_iterator rend() const noexcept { return crend(); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator{cend()}; }

// --- capacity
    bool empty() const noexcept { return head.next == &tail; }
    size_type size() const noexcept
    { return std::count_if(cbegin(),cend(),[](const_reference){return true;}); }

// --- modifiers
    void clear() noexcept { free(); }
    iterator insert( const_iterator pos, const T& value )
    {
        auto newnode = make_node(value);
        link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                  , const_cast<node*>(pos.base) );
        return iterator(newnode);
    }
    iterator insert( const_iterator pos, T&& value )
    {
        auto newnode = make_node(std::move(value));
        link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                  , const_cast<node*>(pos.base) );
        return iterator(newnode);
    }
    template<typename InputIt>
    iterator insert( const_iterator pos, InputIt first, InputIt last )
    {
        auto nodes = alloc_range(first, last);
        link_nodes( const_cast<node*>(pos.base->prev), nodes
                  , const_cast<node*>(pos.base) );
        return iterator(nodes.first);
    }
    iterator insert( const_iterator pos, std::initializer_list<T> ilist )
    {
        auto nodes = alloc_range(ilist.begin(), ilist.end());
        link_nodes( const_cast<node*>(pos.base->prev), nodes
                  , const_cast<node*>(pos.base) );
        return iterator(nodes.first);
    }
    template<typename... Args>
    iterator emplace( const_iterator pos, Args&&... args )
    {
        auto newnode = make_node(std::forward<Args>(args)...);
        link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                  , const_cast<node*>(pos.base) );
        return iterator(newnode);
    }

    iterator erase( const_iterator pos )
    { Expects(pos != cend());
        auto target = const_cast<node*>(pos.base);
        link_nodes(target->prev, target->next);
        auto ret = target->next;
        free(target);
        return iterator(ret);
    }
    iterator erase( const_iterator first, const_iterator last )
    {
        auto begin = const_cast<node*>(first.base);
        auto end = const_cast<node*>(last.base);
        link_nodes(begin->prev, end);
        free(begin, end);
        return iterator(end);
    }

    void push_back( const T& value )
    {
        auto nn = make_node(value);
        link_nodes(tail.prev, {nn, nn}, &tail);
    }
    void push_back( T&& value )
    {
        auto nn = make_node(std::move(value));
        link_nodes(tail.prev, {nn,nn}, &tail);
    }
    template<typename... Args>
    reference emplace_back( Args&&... args )
    {
        auto nn = make_node(std::forward<Args>(args)...);
        link_nodes(tail.prev, {nn,nn}, &tail);
        return nn->data;
    }
    void pop_back();

    void push_front( const T& value );
    void push_front( T&& value );
    template<typename... Args>
    reference emplace_front( Args&&... args );
    void pop_front();

    void resize( size_type count );
    void resize( size_type count, const T& value );

    void swap( DoubleLinkedList& other ) noexcept;

    friend inline void swap( DoubleLinkedList& lhs, DoubleLinkedList& rhs ) noexcept
    {
        using std::swap;
        swap(lhs.alloc, rhs.alloc);
        swap(lhs.head, rhs.head);
        swap(lhs.tail, rhs.tail);
    }

protected:
    template<typename... Args>
    inline node* make_node( Args&&... args )
    {
        auto nn = alloc.allocate(1);
        alloc.construct(nn, std::forward<Args>(args)...);

        Ensures(nn != nullptr);
        return nn;
    }

    inline void link_nodes( not_null<node*> pred, not_null<node*> succ ) noexcept
    {
        pred->next = succ;
        succ->prev = pred;
    }

    inline void link_nodes( not_null<node*> pred
                          , std::pair<node*,node*> range
                          , not_null<node*> succ ) noexcept
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
    std::pair<node*,node*> alloc_range( InputIt begin, InputIt end )
    { Expects(begin != nullptr && end != nullptr);
        auto first = make_node(*begin);
        auto pred = first;
        while( ++begin != end ){
            auto succ = make_node(*begin);
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

    void free( node* first, node* last ) noexcept
    { Expects(first != nullptr);
        while( first != last ){
            auto temp = first;
            first = first->next;
            alloc.destroy(temp);
            alloc.deallocate(temp,1);
        }
    }

    void free( node* n ) noexcept
    {
        alloc.destroy(n);
        alloc.deallocate(n,1);
    }
private:
    Allocator alloc{Allocator()};
    node head{};
    node tail{};
};
/* ------------------------------------------------------------------------- */
class Invariant_violation_exception : public std::logic_error
{
    using std::logic_error::logic_error;
};

template<typename T>
bool ensure_invariant( const DoubleLinkedList<T>& object )
{
    if( object.head.next == nullptr || object.tail.prev == nullptr )
        throw Invariant_violation_exception("Head or tail point to a nullptr");
    if( (object.head.next == &object.tail &&
         object.tail.prev == &object.head)  // empty list
        ||
        (object.head.next->prev == &object.head &&
         object.tail.prev->next == &object.tail) ) // non-empty list
        return true;
    else
        throw Invariant_violation_exception("Empty or non-empty invariant violated");
}

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

template<typename U>
friend bool operator==(const DLL_const_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_const_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs);
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
    
template<typename U>
friend bool operator==(const DLL_const_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_const_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator==(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs);
template<typename U>
friend bool operator!=(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs);

private:
    const node* base;
};

template<typename U>
bool operator==(const DLL_const_iterator<U>& lhs
               ,const DLL_const_iterator<U>& rhs)
{ return lhs.base == rhs.base; }

template<typename U>
bool operator!=(const DLL_const_iterator<U>& lhs
               ,const DLL_const_iterator<U>& rhs)
{ return lhs.base != rhs.base; }

template<typename U>
bool operator==(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs)
{ return lhs.base == rhs.base; }

template<typename U>
bool operator!=(const DLL_iterator<U>& lhs, const DLL_const_iterator<U>& rhs)
{ return lhs.base != rhs.base; }

template<typename U>
bool operator==(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs)
{ return lhs.base == rhs.base; }

template<typename U>
bool operator!=(const DLL_const_iterator<U>& lhs, const DLL_iterator<U>& rhs)
{ return lhs.base != rhs.base; }

template<typename U>
bool operator==(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs)
{ return lhs.base == rhs.base; }

template<typename U>
bool operator!=(const DLL_iterator<U>& lhs, const DLL_iterator<U>& rhs)
{ return lhs.base != rhs.base; }

/* ------------------------------------------------------------------------- */
#endif /* DOUBLELINKEDLIST_HPP_ */
