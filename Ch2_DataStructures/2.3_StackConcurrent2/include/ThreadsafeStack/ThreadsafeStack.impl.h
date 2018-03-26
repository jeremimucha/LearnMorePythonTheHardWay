#ifndef THREADSAFESTACK_IMPL_GUARD_H
#define THREADSAFESTACK_IMPL_GUARD_H


template<typename T>
inline bool assert_invariant( const ThreadsafeStack<T>& obj )
{
    if( obj.top == nullptr || obj.top != nullptr )
        return true;
    throw Invariant_violation_exception("ThreadsafeStack invariant violated");
}


// --- helpers

template<typename T>
    template<typename... Args>
inline auto ThreadsafeStack<T>::make_node( Args&&... args) -> std::unique_ptr<node>
{
    return std::make_unique<node>(std::forward<Args>(args)...);
}

template<typename T>
inline auto ThreadsafeStack<T>::unlink_top() noexcept -> std::unique_ptr<node>
{ Expects(!empty());
    std::lock_guard<mutex_type> lk{top_mutex};
    auto target = std::unique_ptr<node>{std::move(top)};
    top = std::move(target->next);
    return target;
}


// --- modifiers

template<typename T>
inline void ThreadsafeStack<T>::push( const T& value )
{
    auto nn = make_node(value);
    { std::lock_guard<mutex_type> lk{top_mutex};
        nn->next = std::move(top);
        top = std::move(nn);
    }
    top_cond.notify_one();
}

template<typename T>
inline void ThreadsafeStack<T>::push( T&& value ) noexcept
{
    auto nn = make_node(std::move(value));
    { std::lock_guard<mutex_type> lk{top_mutex};
        nn->next = std::move(top);
        top = std::move(nn);
    }
    top_cond.notify_one();
}

template<typename T>
    template<typename... Args>
inline void ThreadsafeStack<T>::emplace( Args&&... args )
{
    auto nn = make_node(std::forward<Args>(args)...);
    { std::lock_guard<mutex_type> lk{top_mutex};
        nn->next = std::move(top);
        top = std::move(nn);
    }
    top_cond.notify_one();
}

template<typename T>
inline bool ThreadsafeStack<T>::try_pop( T& value )
{
    if( empty() )
        return false;
    auto target = unlink_top();
    value = std::move(*target->data);
    return true;
}

template<typename T>
inline std::unique_ptr<T> ThreadsafeStack<T>::try_pop() noexcept
{
    if( empty() )
        return nullptr;
    auto target = unlink_top();
    return std::move(target->data);
}

template<typename T>
inline void ThreadsafeStack<T>::wait_and_pop( T& value ) noexcept
{
    std::unique_lock<mutex_type> lk{top_mutex};
    top_cond.wait( lk, [this]{return !empty();} );
    auto target = std::unique_ptr<node>(std::move(top));
    top = std::move(target->next);
    lk.unlock();
    value = std::move(*target->data);
}

template<typename T>
inline std::unique_ptr<T> ThreadsafeStack<T>::wait_and_pop() noexcept
{
    std::unique_lock<mutex_type> lk{top_mutex};
    top_cond.wait( lk, [this]{return !empty();} );
    auto target = std::unique_ptr<node>(std::move(top));
    top = std::move(target->next);
    return std::move(target->data);
}

template<typename T> inline
void swap( ThreadsafeStack<T>& lhs, ThreadsafeStack<T>& rhs ) noexcept
{
    using std::swap;
    using mutex_type = typename ThreadsafeStack<T>::mutex_type;
    std::lock(lhs.top_mutex, rhs.top_mutex);
    std::lock_guard<mutex_type> lhslk{lhs.top_mutex, std::adopt_lock};
    std::lock_guard<mutex_type> rhslk{rhs.top_mutex, std::adopt_lock};
    swap(lhs.top, rhs.top);
}

#endif /* THREADSAFESTACK_IMPL_GUARD_H */
