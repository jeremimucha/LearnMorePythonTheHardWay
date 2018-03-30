#ifndef QUEUE_IMPL_GUARD_H_
#define QUEUE_IMPL_GUARD_H_


template<typename T>
    template<typename... Args>
inline auto Queue<T>::make_node( Args&&... args ) -> node*
{
    auto* const nn = alloc.allocate(1);
    alloc.construct(nn, &head_, std::forward<Args>(args)...);
    return nn;
}

template<typename T>
inline auto Queue<T>::alloc_range( const node_base* begin, const node_base* end ) -> std::pair<node*,node*>
{ Expects(begin != end);
    auto* const first = make_node(static_cast<const node*>(begin)->data);
    auto pred = first;
    begin = begin->next;
    while( begin != end ){
        auto* const succ = make_node(static_cast<const node*>(begin)->data);
        pred->next = succ;
        pred = succ;
        begin = begin->next;
    }
    return {first, pred};
}

template<typename T>
inline void Queue<T>::free( not_null<node*> target ) noexcept
{
    alloc.destroy(target.get());
    alloc.deallocate(target.get(),1);
}

template<typename T>
inline void Queue<T>::free( node* first, node* last ) noexcept
{
    while( first != last ){
        auto* const target = first;
        first = static_cast<node*>(target->next);
        free(target);
    }
}

template<typename T>
inline void Queue<T>::free() noexcept
{
    free( static_cast<node*>(head_.next), static_cast<node*>(&head_) );
}

template<typename T>
Queue<T>::Queue( const Queue& other )
    : QueueBase{}
{
    if(other.empty())
        return;
    auto nodes = alloc_range(other.head_.next, &other.head_);
    head_.next = nodes.first;
    tail_ = nodes.second;
}
template<typename T>
Queue<T>::Queue( Queue&& other ) noexcept
    : QueueBase{other.head_.next, other.tail_}
{
    other.tail_ = other.head_.next = &other.head_;
}
// template<typename T>
// Queue& Queue<T>::operator=(const Queue& ) = delete;
// template<typename T>
// Queue& Queue<T>::operator=( Queue&& ) noexcept = delete;

template<typename T>
auto Queue<T>::front() -> reference
{ Expects(head_.next != &head_);
    return static_cast<node*>(get_front_())->data;
}
template<typename T>
auto Queue<T>::front() const -> const_reference
{ Expects(head_.next != &head_);
    return static_cast<const node*>(get_front_())->data;
}
template<typename T>
auto Queue<T>::back() -> reference
{ Expects(tail_ != &head_);
    return static_cast<node*>(get_back_())->data;
}
template<typename T>
auto Queue<T>::back() const -> const_reference
{ Expects(tail_ != &head);
    return static_cast<const node*>(get_back_())->data;
}

template<typename T>
void Queue<T>::push( const T& value )
{
    auto* const nn = make_node(value);
    push_back_(nn);
}
template<typename T>
void Queue<T>::push( T&& value )
{
    auto* const nn = make_node(std::move(value));
    push_back_(nn);
}

template<typename T>
    template<typename... Args>
decltype(auto) Queue<T>::emplace( Args&&... args )
{
    auto* const nn = make_node(std::forward<Args>(args)...);
    push_back_(nn);
    return nn->data;
}

template<typename T>
void Queue<T>::pop()
{ Expects(head_.next != &head_);
    auto freenode = [this](node* n){ alloc.destroy(n); alloc.deallocate(n,1); };
    auto target = std::unique_ptr<node,decltype(freenode)>(
                                static_cast<node*>(unlink_head_()), freenode );

}

template<typename T>
inline void swap( Queue<T>& lhs, Queue<T>& rhs ) noexcept
{
    using std::swap;
    swap(lhs.alloc, rhs.alloc);
    swap(lhs.head_, rhs.head_);
    swap(lhs.tail_, rhs.tail_);
}


#endif /* QUEUE_IMPL_GUARD_H_ */
