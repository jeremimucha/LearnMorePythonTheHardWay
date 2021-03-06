#ifndef SINGLE_LINKED_LIST_HPP
#define SINGLE_LINKED_LIST_HPP

#include <memory>
#include <utility>
#include <iterator>
#include <initializer_list>
#include <type_traits>

#define GSL_THROW_ON_CONTRACT_VIOLATION
#include <gsl/gsl>
#if defined(NDEBUG)
#undef Expects
#undef Ensures
#define Expects(cond)
#define Ensures(cond)
#endif

using gsl::not_null;

template<typename> class Node;
template<typename> class SingleLinkedList;
template<typename> class SLL_iterator;
template<typename> class SLL_const_iterator;

/* SLL Node<T> */
/* ------------------------------------------------------------------------- */
template<typename T>
struct Node
{
friend class SingleLinkedList<T>;
friend class SLL_iterator<T>;
friend class SLL_const_iterator<T>;

    Node() noexcept = default;
    template<typename... Args>
    explicit Node( Args&&... args )
        : data{ std::forward<Args>(args)... } { }

    T        data;
    Node*    next{nullptr};
};
/* ------------------------------------------------------------------------- */

/* SingleLinkedList */
/* ------------------------------------------------------------------------- */
template<typename T>
class SingleLinkedList
{
    using node                      = Node<T>;
    using Allocator                 = std::allocator<node>;
    using T_Alloc                   = std::allocator<T>;
    using alloc_traits              = std::allocator_traits<Allocator>;
    using T_alloc_traits            = std::allocator_traits<T_Alloc>;
    using self                      = SingleLinkedList;
public:
    using allocator_type            = Allocator;
    using value_type                = typename T_alloc_traits::value_type;
    using reference                 = value_type&;
    using const_reference           = const value_type&;
    using pointer                   = typename T_alloc_traits::pointer;
    using const_pointer             = typename T_alloc_traits::const_pointer;
    using difference_type           = typename T_alloc_traits::difference_type;
    using size_type                 = typename T_alloc_traits::size_type;

    using iterator                  = SLL_iterator<T>;
    using const_iterator            = SLL_const_iterator<T>;

// --- constructors
    SingleLinkedList() noexcept = default;
    explicit SingleLinkedList( size_type count )
        {
            auto nodes = alloc_n(count);
            link_nodes(&head,  nodes.first);
        }

    SingleLinkedList( size_type count, const T& value )
        {
            auto nodes = alloc_n(count, value);
            link_nodes(&head, nodes.first);
        }

    SingleLinkedList( std::initializer_list<T> ilist )
        {
            auto nodes = alloc_range(ilist.begin(), ilist.end());
            link_nodes(&head, nodes.first);
        }

// --- destructor
    ~SingleLinkedList() noexcept { free(); }

// --- copy control
    SingleLinkedList( const SingleLinkedList& other )
        : alloc{other.alloc}
        {
            auto nodes = alloc_range(other.cbegin(), other.cend());
            link_nodes(&head,nodes.first);
        }
    SingleLinkedList( SingleLinkedList&& other ) noexcept
        : alloc{std::move(other.alloc)}
        {
            link_nodes(&head, other.head.next);
            other.head.next = nullptr;
        }

    SingleLinkedList& operator=( const SingleLinkedList& other )
        {
            // if(this == &other) return *this;
            auto nodes = alloc_range(other.cbegin(), other.cend());
            free();
            alloc = other.alloc;
            link_nodes(&head, nodes.first);
            return *this;
        }
    SingleLinkedList& operator=( SingleLinkedList&& other) noexcept
        {
            auto temp = head.next;
            link_nodes(&head, other.head.next);
            alloc = std::move(other.alloc);
            // other is about to be destroyed let it handle freeing of our old data
            link_nodes(&other.head, temp);
            return *this;
        }
    SingleLinkedList& operator=( std::initializer_list<T> ilist )
        {
            auto nodes = alloc_range(ilist.begin(), ilist.end());
            free();
            link_nodes(&head, nodes.first);
            return *this;
        }

// --- iterators
    iterator       before_begin() noexcept { return iterator{&head}; }
    const_iterator before_begin() const noexcept { return const_iterator{&head}; }
    const_iterator cbefore_begin() const noexcept { return const_iterator{&head}; }
    iterator       begin() noexcept { return iterator{head.next}; }
    const_iterator begin() const noexcept { return const_iterator{head.next}; }
    const_iterator cbegin() const noexcept { return const_iterator{head.next}; }
    iterator       end() noexcept { return iterator{nullptr}; }
    const_iterator end() const noexcept { return const_iterator{nullptr}; }
    const_iterator cend() const noexcept { return const_iterator{nullptr}; }

// --- element accesss
    reference front() { return head.next->data; }
    const_reference front() const { return head.next->data; }

// --- capacity
    bool empty() const noexcept { return head.next == nullptr; }
    // This is relatively expensive - O(n) - compared to how easy it is to use
    // the C++ standard doesn't provide size() for lists.
    size_type size() const noexcept
    { return std::count_if(cbegin(), cend(), [](const_reference){return true;} ); }

// --- modifiers
    void clear()  noexcept { free(); }
    iterator insert_after( const_iterator pos, const T& value );
    iterator insert_after( const_iterator pos, T&& vlaue );
    iterator insert_after( const_iterator pos, size_type count, const T& value );
    template<typename InputIt
            , typename = std::enable_if_t<
                std::is_base_of_v<std::input_iterator_tag,
                    typename std::iterator_traits<InputIt>::iterator_category
                >
              >
            >
    iterator insert_after( const_iterator pos, InputIt first, InputIt last );
    iterator insert_after( const_iterator pos, std::initializer_list<T> ilist );
    
    template<typename... Args>
    iterator emplace_after( const_iterator pos, Args&&... args);

    iterator erase_after( const_iterator pos );
    iterator erase_after( const_iterator first, const_iterator last );

    void push_front( const T& value )
        {
            auto data = make_node(value);
            link_nodes(data, head.next);
            link_nodes(&head, data);
            
            Ensures(head.next != nullptr);
        }

    void push_front( T&& value )
        {
            auto data = make_node(std::move(value));
            link_nodes(data, head.next);
            link_nodes(&head, data);

            Ensures(head.next != nullptr);
        }

    template<typename... Args>
    reference emplace_front(Args&&... args)
        {
            auto data = make_node(std::forward<Args>(args)...);
            link_nodes(data, head.next);
            link_nodes(&head, data);

            Ensures(head.next != nullptr);
            return head.next->data;
        }

    void pop_front()
        {   Expects(head.next != nullptr);
            
            auto temp = head.next;
            link_nodes(&head, temp->next);
            alloc.destroy(temp);
            alloc.deallocate(temp,1);
        }

    void resize( size_type count );
    void resize( size_type count, const value_type& value );

    void swap( SingleLinkedList& other ) noexcept
        {
            using std::swap;
            swap(alloc, other.alloc);
            swap(head.next, other.head.next);
        }

    friend inline void swap( SingleLinkedList& lhs, SingleLinkedList& rhs ) noexcept
        {
            using std::swap;
            swap(lhs.alloc, rhs.alloc);
            swap(lhs.head.next, rhs.head.next);
        }

protected:
    template<typename... Args>
    inline node* make_node(Args&&... args);

    template<typename... Args>
    inline std::pair<node*, node*> alloc_n( size_type n, Args&&... args );

    template<typename Iter>
    inline std::pair<node*,node*> alloc_range(Iter begin, Iter end);

    inline void link_nodes( not_null<node*> pred, node* succ );

    void free() noexcept;
private:
    Allocator alloc{Allocator()};
    node head{};
};
/* ------------------------------------------------------------------------- */


/* Iterators */
/* ------------------------------------------------------------------------- */
template<typename T>
class SLL_iterator
{
    friend class SingleLinkedList<T>;
    using Container         = SingleLinkedList<T>;
    using self              = SLL_iterator;
public:
    using node              = Node<T>;
    using value_type        = typename Container::value_type;
    using reference         = typename Container::reference;
    using pointer           = typename Container::pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = typename Container::difference_type;
    using const_iterator    = SLL_const_iterator<T>;

    explicit SLL_iterator( node* n ) noexcept
        : m_ptr{n} { }

    reference operator*() const noexcept
        { return m_ptr->data; }
    
    pointer operator->() const noexcept
        { return &(m_ptr->data); }
    
    self& operator++() noexcept
        {
            m_ptr = m_ptr->next;
            return *this;
        }
    self operator++(int) noexcept
        {
            auto rv = *this;
            m_ptr = m_ptr->next;
            return rv;
        }
    
    operator const_iterator()
        { return const_iterator(m_ptr); }

    bool operator==( const self& other ) const noexcept
        { return m_ptr == other.m_ptr; }
    bool operator!=( const self& other ) const noexcept
        { return m_ptr != other.m_ptr; }

template<typename U> friend bool operator==( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept;
template<typename U> friend bool operator!=( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept;

template<typename U> friend bool operator==( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept;
template<typename U> friend bool operator!=( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept;
private:
    node* m_ptr;
};

template<typename T>
class SLL_const_iterator
{
    friend class SingleLinkedList<T>;
    using Container         = SingleLinkedList<T>;
    using self              = SLL_const_iterator;
public:
    using node              = Node<T>;
    using value_type        = typename Container::value_type;
    using reference         = typename Container::const_reference;
    using pointer           = typename Container::const_pointer;
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = typename Container::difference_type;

    explicit SLL_const_iterator( node* n )
        : m_ptr{n} { }

    reference operator*() const noexcept
        { return m_ptr->data; }
    
    pointer operator->() const noexcept
        { return &(m_ptr->data); }
    
    self& operator++() noexcept
        {
            m_ptr = m_ptr->next;
            return *this;
        }
    self operator++(int) noexcept
        {
            auto rv = *this;
            m_ptr = m_ptr->next;
            return rv;
        }

    bool operator==( const self& other ) const noexcept
        { return m_ptr == other.m_ptr; }
    bool operator!=( const self& other ) const noexcept
        { return m_ptr != other.m_ptr; }

template<typename U> friend bool operator==( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept;
template<typename U> friend bool operator!=( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept;

template<typename U> friend bool operator==( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept;
template<typename U> friend bool operator!=( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept;
private:
    const node* m_ptr;
};

template<typename U>
inline bool operator==( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept
{ return lhs.m_ptr == rhs.m_ptr; }
template<typename U>
inline bool operator!=( const SLL_iterator<U>& lhs
                      , const SLL_const_iterator<U>& rhs) noexcept
{ return lhs.m_ptr != rhs.m_ptr; }

template<typename U>
inline bool operator==( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept
{ return lhs.m_ptr == rhs.m_ptr; }
template<typename U>
inline bool operator!=( const SLL_const_iterator<U>& lhs
                      , const SLL_iterator<U>& rhs) noexcept
{ return lhs.m_ptr != rhs.m_ptr; }
/* ------------------------------------------------------------------------- */

/* SingleLinkedList */
/* ------------------------------------------------------------------------- */
template<typename T>
    template<typename... Args>
inline auto
SingleLinkedList<T>::make_node(Args&&... args) -> node*
{
    auto data = alloc.allocate(1);
    alloc.construct(data, std::forward<Args>(args)...);
    
    Ensures(data != nullptr);
    Ensures(data->next == nullptr);
    return data;
}

template<typename T>
    template<typename... Args>
inline auto
SingleLinkedList<T>::alloc_n( size_type n, Args&&... args ) -> std::pair<node*,node*>
{   
    Expects(n > 0);
    // TODO: Figure out how to allocate a whole chunk of memory
    // and then be able to deallocate it...
    // auto first = alloc.allocate(n);
    // alloc.construct(first, std::forward<Args>(args)...);
    // auto pred = first;
    // for( auto succ = pred; succ != first + n; pred = succ, ++succ){
    //     alloc.construct(succ, std::forward<Args>(args)...);
    //     pred->next = succ;
    // }
    auto first = make_node(std::forward<Args>(args)...);
    auto pred = first;
    while( --n ){
        auto succ = make_node(std::forward<Args>(args)...);
        link_nodes(pred, succ);
        pred = succ;
    }

    Ensures(pred->next == nullptr);
    return {first, pred};
}

template<typename T>
    template<typename Iter>
inline auto
SingleLinkedList<T>::alloc_range(Iter begin, Iter end) -> std::pair<node*,node*>
{   
    // Expects() ?
    // TODO: Figure out how to allocate a whole chunk of memory
    // and then be able to deallocate it...
    // auto count = std::distance(begin, end);
    // auto first = alloc.allocate(count);
    // alloc.construct(first, *begin);
    // auto pred = first;
    // for( auto succ = pred; begin != end; ){
    //     alloc.construct(succ, *++begin);
    //     pred->next = succ;
    // }
    auto first = make_node(*begin);
    auto pred = first;
    while( ++begin != end ){
        auto succ = make_node(*begin);
        link_nodes(pred, succ);
        pred = succ;
    }

    Ensures(pred->next == nullptr);
    return {first, pred};
}

template<typename T>
inline void
SingleLinkedList<T>::link_nodes( not_null<node*> pred, node* succ )
{
    pred->next = succ;
}

template<typename T>
inline void
SingleLinkedList<T>::free() noexcept
{
    while(head.next != nullptr){
        auto temp = head.next;
        head.next = temp->next;
        alloc.destroy(temp);
        alloc.deallocate(temp,1);
    }
}

template<typename T>
auto SingleLinkedList<T>::insert_after( const_iterator pos, const T& value ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_node = make_node(value);
    link_nodes(new_node, pred->next);
    link_nodes(pred, new_node);
    return iterator(new_node);
}
template<typename T>
auto SingleLinkedList<T>::insert_after( const_iterator pos, T&& value ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_node = make_node(std::move(value));
    link_nodes(new_node, pred->next);
    link_nodes(pred, new_node);
    return iterator(new_node);
}
template<typename T>
auto SingleLinkedList<T>::insert_after( const_iterator pos, size_type count, const T& value ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_nodes = alloc_n(count, value);
    link_nodes(new_nodes.second, pred->next);
    link_nodes(pred, new_nodes.first);
    return iterator(new_nodes.first);
}
template<typename T>
    template<typename InputIt
        , typename
        >
auto SingleLinkedList<T>::insert_after( const_iterator pos, InputIt first, InputIt last ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_nodes = alloc_range(first, last);
    link_nodes(new_nodes.second, pred->next);
    link_nodes(pred, new_nodes.first);
    return iterator(new_nodes.first);
}
template<typename T>
auto SingleLinkedList<T>::insert_after( const_iterator pos, std::initializer_list<T> ilist ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_nodes = alloc_range(ilist.begin(), ilist.end());
    link_nodes(new_nodes.second, pred->next);
    link_nodes(pred, new_nodes.first);
    return iterator(new_nodes.first);
}

template<typename T>
    template<typename... Args>
auto SingleLinkedList<T>::emplace_after( const_iterator pos, Args&&... args) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto pred = const_cast<node*>(pos.m_ptr);
    auto new_node = make_node(std::forward<Args>(args)...);
    link_nodes(new_node, pred->next);
    link_nodes(pred, new_node);
    return iterator(new_node);
}

template<typename T>
auto SingleLinkedList<T>::erase_after( const_iterator pos ) -> iterator
{
    auto pred = const_cast<node*>(pos.m_ptr);
    auto nodedtor = [this](node* pn){ alloc.destroy(pn); alloc.deallocate(pn,1); };
    if(pred->next){
        auto freenode = std::unique_ptr<node,decltype(nodedtor)>(pred->next, nodedtor);
        link_nodes(pred, pred->next->next);
        return iterator(pred->next);
    }
    else { return end(); }
}
template<typename T>
auto SingleLinkedList<T>::erase_after( const_iterator first, const_iterator last ) -> iterator
{ Expects(pos.m_ptr != nullptr);
    auto b = const_cast<node*>(first.m_ptr);
    auto e = const_cast<node*>(last.m_ptr);
    link_nodes(b, e);
    for( b = b->next; b != e ; ){
        auto temp = b;
        b = b->next;
        alloc.destroy(temp);
        alloc.deallocate(temp, 1);
    }
    return iterator(e);
}
template<typename T>
void SingleLinkedList<T>::resize( size_type count )
{
    self::resize(count, T{});
}
template<typename T>
void SingleLinkedList<T>::resize( size_type count, const value_type& value )
{
    // find the current count + last element
    auto last_n_size = [count = count, pn = head.next]()mutable{
        auto current_count = size_type{0};
        auto last = pn;
        for(; pn != nullptr && current_count < count; last = pn, pn = pn->next){
            ++current_count;
        }
        return std::make_pair(last, current_count);
    }();

    if( last_n_size.second != count ){
        // allocate elements to reach desired count
        auto new_nodes = alloc_n(count - last_n_size.second, value);
        link_nodes(last_n_size.first, new_nodes.first);
    }
    else{
        // free the excess elements
        for( auto n = last_n_size.first; n != nullptr; ){
            auto temp = n;
            n = n->next;
            alloc.destroy(temp);
            alloc.deallocate(temp, 1);
        }
    }
}
/* ------------------------------------------------------------------------- */


#endif /* SINGLE_LINKED_LIST_HPP */
