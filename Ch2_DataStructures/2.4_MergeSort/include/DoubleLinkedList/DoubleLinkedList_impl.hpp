#ifndef DOUBLELINKEDLIST_IMPL_INCLUDED_HPP_
#define DOUBLELINKEDLIST_IMPL_INCLUDED_HPP_

// This is an implementation file intented strictly to be included by
// DoubleLinkedList.hpp header
#include <iostream>

template<typename T>
auto DoubleLinkedList<T>::operator=( const DoubleLinkedList& other ) -> DoubleLinkedList&
{
// potentially very expensive, should  we guard against self assignment
// rather than do a self-assignment safe implementation?
    alloc = other.alloc;
    auto nodes = alloc_range(other.cbegin(), other.cend());
    free();
    link_nodes(&head, nodes, &tail);
    return *this;
}
template<typename T> inline
auto DoubleLinkedList<T>::operator=( DoubleLinkedList&& other ) noexcept -> DoubleLinkedList&
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
template<typename T>
auto DoubleLinkedList<T>::operator=( std::initializer_list<T> ilist ) -> DoubleLinkedList&
{
    auto nodes = alloc_range(ilist.begin(), ilist.end());
    free();
    link_nodes(&head, nodes, &tail);
    return *this;
}


template<typename T> inline
auto DoubleLinkedList<T>::insert( const_iterator pos, const T& value ) -> iterator
{
    auto newnode = make_node(value);
    link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                , const_cast<node*>(pos.base) );
    return iterator(newnode);
}
template<typename T> inline
auto DoubleLinkedList<T>::insert( const_iterator pos, T&& value ) -> iterator
{
    auto newnode = make_node(std::move(value));
    link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                , const_cast<node*>(pos.base) );
    return iterator(newnode);
}
template<typename T>
    template<typename InputIt, typename>
auto DoubleLinkedList<T>::insert( const_iterator pos, InputIt first, InputIt last ) -> iterator
{
    auto nodes = alloc_range(first, last);
    link_nodes( const_cast<node*>(pos.base->prev), nodes
                , const_cast<node*>(pos.base) );
    return iterator(nodes.first);
}
template<typename T>
auto DoubleLinkedList<T>::insert( const_iterator pos, std::initializer_list<T> ilist ) -> iterator
{
    auto nodes = alloc_range(ilist.begin(), ilist.end());
    link_nodes( const_cast<node*>(pos.base->prev), nodes
                , const_cast<node*>(pos.base) );
    return iterator(nodes.first);
}
template<typename T>
    template<typename... Args> inline
auto DoubleLinkedList<T>::emplace( const_iterator pos, Args&&... args ) -> iterator
{
    auto newnode = make_node(std::forward<Args>(args)...);
    link_nodes( const_cast<node*>(pos.base->prev), {newnode, newnode}
                , const_cast<node*>(pos.base) );
    return iterator(newnode);
}

template<typename T>
auto DoubleLinkedList<T>::erase( const_iterator pos ) -> iterator
{ Expects(pos != cend());
    auto target = const_cast<node*>(pos.base);
    link_nodes(target->prev, target->next);
    auto ret = target->next;
    free(target);
    return iterator(ret);
}
template<typename T>
auto DoubleLinkedList<T>::erase( const_iterator first, const_iterator last ) -> iterator
{
    auto begin = const_cast<node*>(first.base);
    auto end = const_cast<node*>(last.base);
    link_nodes(begin->prev, end);
    free(begin, end);
    return iterator(end);
}

template<typename T> inline
void DoubleLinkedList<T>::push_back( const T& value )
{
    auto nn = make_node(value);
    link_nodes(tail.prev, {nn, nn}, &tail);
}

template<typename T> inline
void DoubleLinkedList<T>::push_back( T&& value )
{
    auto nn = make_node(std::move(value));
    link_nodes(tail.prev, {nn,nn}, &tail);
}

template<typename T>
    template<typename... Args> inline
auto DoubleLinkedList<T>::emplace_back( Args&&... args ) -> reference
{
    auto nn = make_node(std::forward<Args>(args)...);
    link_nodes(tail.prev, {nn,nn}, &tail);
    return nn->data;
}

template<typename T> inline
void DoubleLinkedList<T>::pop_back()
{
    auto deleter = [this](node* np){ free(np); };
    std::unique_ptr<node,decltype(deleter)> target{tail.prev, deleter};
    link_nodes(tail.prev->prev, &tail);
}

template<typename T> inline
void DoubleLinkedList<T>::push_front( const T& value )
{
    auto nn = make_node(value);
    link_nodes(&head, {nn, nn}, head.next);
}

template<typename T> inline
void DoubleLinkedList<T>::push_front( T&& value )
{
    auto nn = make_node(std::move(value));
    link_nodes(&head, {nn,nn}, head.next);
}

template<typename T>
    template<typename... Args> inline
auto DoubleLinkedList<T>::emplace_front( Args&&... args ) -> reference
{
    auto nn = make_node(std::forward<Args>(args)...);
    link_nodes(&head, {nn,nn}, head.next);
    return nn->data;
}

template<typename T> inline
void DoubleLinkedList<T>::pop_front()
{
    auto deleter = [this](node* np){ free(np); };
    std::unique_ptr<node,decltype(deleter)> target{head.next, deleter};
    link_nodes(&head, head.next->next);
}

template<typename T> inline
void DoubleLinkedList<T>::resize( size_type count )
{
    self::resize(count, T{});
}

template<typename T>
void DoubleLinkedList<T>::resize( size_type count, const T& value )
{
    if( count == 0 ){
        free();
        return;
    }

    const auto it_and_size = [begin=head.next, end=&tail, count=count]()mutable{
        size_type actual_size{0};
        while( begin != end && ++actual_size != count ){
            begin = begin->next;
        }
        return std::make_pair(begin, actual_size);
    }();
    
    if( it_and_size.second == count ){ // more or exactly as many nodes
    Expects(it_and_size.first != &tail);
        // free the exces elements
        auto target = it_and_size.first->next;
        link_nodes(it_and_size.first, &tail);
        free(target, &tail);
    }
    else{ // actual_size < desired count
    Expects(it_and_size.first == &tail);
        auto nodes = alloc_n(count - it_and_size.second, value);
        link_nodes(tail.prev, nodes, &tail);
    }
}

template<typename T>
auto DoubleLinkedList<T>::get_count( std::pair<node*,node*> range) noexcept -> size_type
{
    auto count = size_type{0};
    while( range.first != range.second ){
        ++count;
        range.first = range.first->next;
    }
    return count;
}

template<typename T>
auto DoubleLinkedList<T>::get_middle( node* first, size_type count ) noexcept -> node*
{ Expects(count > 1);
    --count;
    count /= 2;
    while(count--){
        first = first->next;
    }
    return first;
}

template<typename T>
auto DoubleLinkedList<T>::do_merge(std::pair<node*,node*> lhs,
        std::pair<node*,node*> rhs) noexcept -> std::pair<node*,node*>
{
    auto res_begin = [&lhs, &rhs]{
                    if(lhs.first->data < rhs.first->data){
                        auto rv = lhs.first;
                        lhs.first = lhs.first->next;
                        return rv;
                    }else{
                        auto rv = rhs.first;
                        rhs.first = rhs.first->next;
                        return rv;
                    }
                  }();
    auto res_end = res_begin;
    while( (lhs.first != lhs.second) && (rhs.first != rhs.second) ){
        if( lhs.first->data < rhs.first->data ){
            link_nodes(res_end, lhs.first);
            res_end = lhs.first;
            lhs.first = lhs.first->next;
        }else{
            link_nodes(res_end, rhs.first);
            res_end = rhs.first;
            rhs.first = rhs.first->next;
        }
    }
    while( lhs.first != lhs.second ){
        link_nodes(res_end, lhs.first);
        res_end = lhs.first;
        lhs.first = lhs.first->next;
    }
    while( rhs.first != rhs.second ){
        link_nodes(res_end, rhs.first);
        res_end = rhs.first;
        rhs.first = rhs.first->next;
    }
    res_end->next = &tail;
    tail.prev = res_end;
    return {res_begin, res_end->next};
}

template<typename T>
auto DoubleLinkedList<T>::do_sort_merge( std::pair<node*,node*> range ) noexcept
     -> std::pair<node*,node*>
{
    const auto count = get_count(range);
    if( count < 2 )
        return range;

    const auto midpoint = get_middle(range.first, count);
    auto right_begin = midpoint->next;
    midpoint->next = &tail;
    auto left = do_sort_merge({range.first, &tail});
    auto right = do_sort_merge({right_begin, &tail});

    return do_merge(left, right);
}

template<typename T>
void DoubleLinkedList<T>::sort_merge() noexcept
{
    auto sorted = do_sort_merge({head.next, &tail});
    link_nodes(&head, sorted.first);
}

template<typename T>
    template<typename... Args> inline
auto DoubleLinkedList<T>::make_node( Args&&... args ) -> node*
{
    auto nn = alloc.allocate(1);
    alloc.construct(nn, std::forward<Args>(args)...);

    Ensures(nn != nullptr);
    return nn;
}

template<typename T> inline
void DoubleLinkedList<T>::link_nodes( not_null<node*> pred, not_null<node*> succ ) noexcept
{
    pred->next = succ;
    succ->prev = pred;
}

template<typename T> inline
void DoubleLinkedList<T>::link_nodes( not_null<node*> pred
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

template<typename T>
    template<typename... Args> inline
auto DoubleLinkedList<T>::alloc_n( size_type n, Args&&... args ) -> std::pair<node*,node*>
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

template<typename T>
    template<typename InputIt> inline
auto DoubleLinkedList<T>::alloc_range( InputIt begin, InputIt end ) -> std::pair<node*,node*>
{
    auto first = make_node(*begin);
    auto pred = first;
    while( ++begin != end ){
        auto succ = make_node(*begin);
        link_nodes(pred, succ);
        pred = succ;
    }
    return {first, pred};
}

template<typename T> inline
void DoubleLinkedList<T>::free() noexcept
{
    while( head.next != &tail ){
        auto temp = head.next;
        head.next = temp->next;
        alloc.destroy(temp);
        alloc.deallocate(temp, 1);
    }
    tail.prev = &head;
}

template<typename T> inline
void DoubleLinkedList<T>::free( node* first, node* last ) noexcept
{ Expects(first != nullptr);
    while( first != last ){
        auto temp = first;
        first = first->next;
        alloc.destroy(temp);
        alloc.deallocate(temp,1);
    }
}

template<typename T> inline
void DoubleLinkedList<T>::free( node* n ) noexcept
{
    alloc.destroy(n);
    alloc.deallocate(n,1);
}


namespace detail
{

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

} // detail

#endif /* DOUBLELINKEDLIST_IMPL_INCLUDED_HPP_ */
