#include "gtest/gtest.h"
#include "ThreadsafeStack/ThreadsafeStack.h"


namespace
{


class ThreadsafeStackTest : public ::testing::Test
{
    void SetUp() override
    {
        assert_invariant(f_stack);
    }
protected:
    ThreadsafeStack<int> f_stack{};
};


TEST_F( ThreadsafeStackTest, DefaultConstructor )
{
    ASSERT_TRUE(true);
}

TEST_F( ThreadsafeStackTest, PushLvalue )
{
    const int value = 42;
    ASSERT_TRUE(f_stack.empty());
    ASSERT_EQ(value, 42);
    f_stack.push(value);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.wait_and_pop(), value);
}


} // namespace
