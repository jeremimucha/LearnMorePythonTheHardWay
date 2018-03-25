#include "gtest/gtest.h"
#include <array>
#include "ThreadsafeStack/ThreadsafeStack.h"


namespace
{


class ThreadsafeStackTest : public ::testing::Test
{
    void SetUp() override
    {
        assert_invariant(f_stack);
        ASSERT_TRUE(f_stack.empty);
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
    ASSERT_EQ(value, 42);
    f_stack.push(value);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.wait_and_pop(), value);
}

TEST_F( ThreadsafeStackTest, PushRvalue )
{
    f_stack.push(42);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.wait_and_pop(), 42);
}

TEST_F( ThreadsafeStackTest, Emplace )
{
    f_stack.emplace(42);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.wait_and_pop(), 42);
}

TEST_F( ThreadsafeStackTest, WaitAndPopReturnByParameter )
{
    const std::array<int, 5> input_array{1,2,3,4,5};
    for( auto i : input_array )
        f_stack.push(i);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    auto value = 0;
    for( auto it = input_array.crbegin(); 
         it != input_array.crend() && !f_stack.empty();
         ++it ){
        f_stack.wait_and_pop(value);
        ASSERT_EQ(value, *it);
    }
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_TRUE(f_stack.empty());
}

TEST_F( ThreadsafeStackTest, WaitAndPopReturnByValue )
{
    const std::array<int, 5> input_array{1,2,3,4,5};
    for( auto i : input_array )
        f_stack.push(i);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    for( auto it = input_array.crbegin(); 
         it != input_array.crend() && !f_stack.empty();
         ++it ){
        const auto value = f_stack.wait_and_pop();
        ASSERT_EQ(value, *it);
    }
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_TRUE(f_stack.empty());
}

TEST_F( ThreadsafeStackTest, TryPop )
{
    const std::array<int, 5> input_array{1,2,3,4,5};
    for( auto i : input_array )
        f_stack.push(i);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    auto value = 0;
    for( auto it = input_array.crbegin(); 
         it != input_array.crend() && !f_stack.empty();
         ++it ){
        ASSERT_TRUE(f_stack.try_pop(value));
        ASSERT_EQ(value, *it);
    }
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_TRUE(f_stack.empty());
}

} // namespace
