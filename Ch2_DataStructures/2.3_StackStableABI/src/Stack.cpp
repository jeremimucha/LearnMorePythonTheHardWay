#include "Stack.h"


bool assert_invariant( const StackBase& obj )
{
    if( obj.m_top == nullptr || obj.m_top != nullptr )
        return true;
    throw Invariant_violation_exception("Stack invariant violated");
}
