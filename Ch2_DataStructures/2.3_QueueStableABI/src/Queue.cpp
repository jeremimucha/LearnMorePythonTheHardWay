#include "Queue.h"


bool assert_invariant( const QueueBase& obj )
{
    if( (obj.tail_ == &obj.head_ && obj.head_.next == &obj.head_) ||
        (obj.tail_ != &obj.head_ && obj.head_.next != &obj.head_) )
        return true;
    throw Invariant_violation_exception("Queue invariant violated");
}