#ifndef THREADSAFESTACK_IMPL_GUARD_HPP_
#define THREADSAFESTACK_IMPL_GUARD_HPP_


template<typename T>
inline bool assert_invariant( const ThreadsafeStack<T>& obj )
{ // What's the invariant here?
    // Stack is either empty -> head.next == nullptr
    // Or it owns one or more nodes -> head.next != nullptr
    if( obj.top.next == nullptr || obj.top.next != nullptr )
        return true;
}

// --- helpers
template<typename T>
    template<typename... Args> inline
auto ThreadsafeStack<T>::make_node( Args&&... args ) -> node*
{
    auto nn = alloc.allocate(1);
    alloc.construct(nn, std::forward<Args>(args)...);
    return nn;
}

template<typename T> inline
void ThreadsafeStack<T>::link_nodes( not_null<node*> pred, node* succ ) noexcept
{
    pred->next = succ;
}

template<typename T> inline
void ThreadsafeStack<T>::free() noexcept
{
    std::lock_guard<mutex_type> lk{top_mutex};
    while(top.next != nullptr){
        auto target = top.next;
        top.next = target->next;
        free(target);
    }
}

template<typename T> inline
void ThreadsafeStack<T>::free( not_null<node*> target ) noexcept
{
    alloc.destroy(target.get());
    alloc.deallocate(target.get(), 1);
}

template<typename T> inline
void ThreadsafeStack<T>::free( node* first, node* last ) noexcept
{
    while( first != last ){
        auto temp = first;
        first = first->next;
        free(temp);
    }
}


// --- modifiers
template<typename T>
void ThreadsafeStack<T>::push( const T& value )
{
    auto nn = make_node(value);
    { std::lock_guard<mutex_type> lk{top_mutex};
        link_nodes(nn, top.next);
        link_nodes(&top, nn);
    }
    top_cond.notify_one();
}

template<typename T>
void ThreadsafeStack<T>::push( T&& value ) noexcept
{
    auto nn = make_node(std::move(value));
    { std::lock_guard<mutex_type> lk{top_mutex};
        link_nodes(nn, top.next);
        link_nodes(&top, nn);
    }
    top_cond.notify_one();
}

template<typename T>
    template<typename... Args>
void ThreadsafeStack<T>::emplace( Args&&... args )
{
    auto nn = make_node(std::forward<Args>(args)...);
    { std::lock_guard<mutex_type> lk{top_mutex};
        link_nodes(nn, top.next);
        link_nodes(&top, nn);
    }
    top_cond.notify_one();
}

template<typename T>
bool ThreadsafeStack<T>::try_pop( T& value ) noexcept
{
    if( empty() )
        return false;
    auto target = unlink_top();
    value = std::move(target->data);
    return true;
}

template<typename T> // TODO: enable_if noexcept move constructible
auto ThreadsafeStack<T>::wait_and_pop() noexcept -> value_type
{ // This implementation requires type T to be noexcept move-constructible
    std::unique_lock<mutex_type> lk{top_mutex};
    top_cond.wait( lk, [this]{return !empty();} );
    auto target = top.next;
    link_nodes(&top, target->next);
    lk.unlock();
    return std::move(target->data);
}

template<typename T> // TODO: enable_if noexcept move assignable
void ThreadsafeStack<T>::wait_and_pop( T& value ) noexcept
{ // This implementation requires type T to be noexcept move-assignable
    std::unique_lock<mutex_type> lk{top_mutex};
    top_cond.wait( lk, [this]{return !empty();} );
    auto target = top.next;
    link_nodes(&top, target->next);
    lk.unlock();
    value = std::move(target->data);
}

template<typename T>
auto ThreadsafeStack<T>::unlink_top() noexcept -> node*
{ Expects(!empty());
    std::lock_guard<mutex_type> lk{top_mutex};
    auto target = top.next;
    link_nodes(&top, target->next);
    // link_nodes(target.next, nullptr); // necessary?
    return target;
}


template<typename T>
inline void swap( ThreadsafeStack<T>& lhs, ThreadsafeStack<T>& rhs ) noexcept
{
    using std::swap;
    using mutex_type = typename ThreadsafeStack<T>::mutex_type;
    std::lock(lhs.top_mutex, rhs.top_mutex);
    std::lock_guard<mutex_type> lhslk{lhs.top_mutex, std::adopt_lock};
    std::lock_guard<mutex_type> rhslk{rhs.top_mutex, std::adopt_lock};
    swap(lhs.alloc, rhs.alloc);
    swap(lhs.top, rhs.top);
}

#endif /* THREADSAFESTACK_IMPL_GUARD_HPP_ */
