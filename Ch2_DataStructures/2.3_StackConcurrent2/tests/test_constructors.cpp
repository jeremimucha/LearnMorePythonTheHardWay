#include "catch/catch.hpp"
#include "ThreadsafeStack/ThreadsafeStack.h"


TEST_CASE( "ThreadsafeStack constructor test", "[constructor]" )
{
    auto cut_stack = ThreadsafeStack<int>{};
    REQUIRE(assert_invariant(cut_stack));
}
