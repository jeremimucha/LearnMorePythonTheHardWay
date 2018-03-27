#ifndef STACK_STABLEABI_IMPL_GUARD_H_
#define STACK_STABLEABI_IMPL_GUARD_H_


template<typename T>
    template<typename... Args>
inline auto Stack<T>::make_node( Args&&... args ) -> node*
{
    auto nn = alloc.allocate(1);
    alloc.construct(nn, std::forward<Args>(args)...);
    return nn;
}

template<typename T>
inline void Stack<T>::free( not_null<node*> target ) noexcept
{
    alloc.destroy(target.get());
    alloc.deallocate(target.get(), 1);
}

template<typename T>
inline void Stack<T>::free( node* first, node* last ) noexcept
{
    while( first != last ){
        auto temp = first;
        first = static_cast<node*>(first->next);
        free(temp);
    }
}

template<typename T>
inline void Stack<T>::free() noexcept
{
    free(static_cast<node*>(m_top), nullptr);
}

// --- capacity
template<typename T>
inline bool Stack<T>::empty() const noexcept
{
    return m_top == nullptr;
}

// --- modifiers
template<typename T>
inline void Stack<T>::push( const T& value )
{
    auto nn = make_node(value);
    base::push_top(nn);
}

template<typename T>
inline void Stack<T>::push( T&& value )
{
    auto nn = make_node(std::move(value));
    base::push_top(nn);
}

template<typename T>
    template<typename... Args>
inline decltype(auto) Stack<T>::emplace( Args&&... args )
{
    auto nn = make_node(std::forward<Args>(args)...);
    base::push_top(nn);
}

template<typename T>
inline void Stack<T>::pop()
{ Expects(m_top != nullptr);
    auto target = base::unlink_top();
    free( static_cast<node*>(target) );
}

template<typename T>
inline void swap( Stack<T>& lhs, Stack<T>& rhs ) noexcept
{
    using std::swap;
    swap(lhs.alloc, rhs.alloc);
    swap(lhs.m_top, rhs.m_top);
}


#endif /* STACK_STABLEABI_IMPL_GUARD_H_ */
