#ifndef THREADSAFE_QUEUE_IMPL_H_
#define THREADSAFE_QUEUE_IMPL_H_


template<typename T>
inline bool assert_invariant( const ThreadsafeQueue<T>& obj )
{
    if( obj.head != nullptr && obj.tail != nullptr)
        return true;
    throw Invariant_violation_exception("ThreadsafeQueue invariant violated");
}

template<typename T>
void ThreadsafeQueue<T>::push( const T& value )
{
    auto new_data{std::make_shared<T>(value)};
    auto pnode{ std::make_unique<node>() };
    { std::lock_guard<mutex_type> tail_lk{tail_mutex};
        tail->data = new_data;
        node* const new_tail{pnode.get()};
        tail->next = std::move(pnode);
        tail = new_tail;
    }
    cond_var.notify_one();
}

template<typename T>
void ThreadsafeQueue<T>::push( T&& value )
{
    auto new_data{std::make_shared<T>(std::move(value))};
    auto pnode{ std::make_unique<node>() };
    { std::lock_guard<mutex_type> tail_lk{tail_mutex};
        tail->data = new_data;
        node* const new_tail{pnode.get()};
        tail->next = std::move(pnode);
        tail = new_tail;
    }
    cond_var.notify_one();
}

template<typename T>
    template<typename... Args>
void ThreadsafeQueue<T>::emplace( Args&&... args )
{
    auto new_data{std::make_shared<T>(std::forward<Args>(args)...)};
    auto pnode{std::make_unique<node>()};
    { std::lock_guard<mutex_type> tail_lk{tail_mutex};
        tail->data = new_data;
        node* const new_tail{pnode.get()};
        tail->next = std::move(pnode);
        tail = new_tail;
    }
    cond_var.notify_one();
}

template<typename T>
inline auto ThreadsafeQueue<T>::get_tail() const -> node*
{
    std::lock_guard<mutex_type> tail_lk{tail_mutex};
    return tail;
}

template<typename T>
auto ThreadsafeQueue<T>::wait_for_data() -> std::unique_lock<mutex_type>
{
    std::unique_lock<mutex_type> head_lk{head_mutex};
    cond_var.wait( head_lk, [this]{ return head.get() != get_tail(); } );
    return std::move(head_lk);
}

template<typename T>
auto ThreadsafeQueue<T>::pop_head() -> std::unique_ptr<node>
{
    auto target{ std::move(head) };
    head = std::move( target->next );
    return target;
}

template<typename T>
auto ThreadsafeQueue<T>::wait_pop_head() -> std::unique_ptr<node>
{
    std::unique_lock<mutex_type> head_lk{wait_for_data()};
    return pop_head();
}

template<typename T>
auto ThreadsafeQueue<T>::wait_pop_head(T& value) -> std::unique_ptr<node>
{
    std::unique_lock<mutex_type> head_lk{wait_for_data()};
    value = std::move( *head->data );
    return pop_head();
}

template<typename T>
auto ThreadsafeQueue<T>::wait_and_pop() -> std::shared_ptr<T>
{
    const auto target{ wait_pop_head() };
    return target->data;
}

template<typename T>
void ThreadsafeQueue<T>::wait_and_pop( T& value )
{
    const auto target{ wait_pop_head(value) };
}

template<typename T>
auto ThreadsafeQueue<T>::try_pop_head() -> std::unique_ptr<node>
{
    std::lock_guard<mutex_type> head_lk{head_mutex};
    if(head.get() == get_tail())
        return nullptr;
    return pop_head();
}

template<typename T>
auto ThreadsafeQueue<T>::try_pop_head(T& value) -> std::unique_ptr<node>
{
    std::lock_guard<mutex_type> head_lk{head_mutex};
    if(head.get() == get_tail())
        return nullptr;
    value = std::move(*head->data);
    return pop_head();
}

template<typename T>
auto ThreadsafeQueue<T>::try_pop() -> std::shared_ptr<T>
{
    auto target{try_pop_head()};
    return target ? target->data : nullptr;
}

template<typename T>
bool ThreadsafeQueue<T>::try_pop( T& value )
{
    const auto target{try_pop_head(value)};
    return target != nullptr;
}


#endif /* THREADSAFE_QUEUE_IMPL_H_ */
