#ifndef THREADSAFESTACK_IMPL_GUARD_H
#define THREADSAFESTACK_IMPL_GUARD_H


template<typename T>
inline bool assert_invariant( const ThreadsafeStack<T>& obj )
{
    if( obj.top == nullptr || obj.top != nullptr )
        return true;
    throw Invariant_violation_exception("ThreadsafeStack invariant violated");
}

#endif /* THREADSAFESTACK_IMPL_GUARD_H */
