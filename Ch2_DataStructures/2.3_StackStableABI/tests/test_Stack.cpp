#include "gtest/gtest.h"
#include "Stack/Stack.h"


namespace
{

class StackAddElementsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        assert_invariant(f_stack);
        ASSERT_TRUE(f_stack.empty());
    }

    Stack<int> f_stack{};
};

class StackRemoveElementsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        assert_invariant(f_stack);
        ASSERT_TRUE(f_stack.empty());
        for( auto i : f_input_array )
            f_stack.push(i);
        ASSERT_TRUE(assert_invariant(f_stack));
        ASSERT_FALSE(f_stack.empty());
    }

    const std::array<int, 5> f_input_array{1,2,3,4,5};
    Stack<int> f_stack{};
};


TEST_F( StackAddElementsTest, DefaultConstructor )
{
    ASSERT_TRUE(true);
}

TEST_F( StackAddElementsTest, PushLvalue )
{
    const int value = 42;
    f_stack.push(value);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.top(), value);
}

TEST_F( StackAddElementsTest, PushRvalue )
{
    f_stack.push(42);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.top(), 42);
}

TEST_F( StackAddElementsTest, Emplace )
{
    f_stack.emplace(42);
    ASSERT_TRUE(assert_invariant(f_stack));
    ASSERT_FALSE(f_stack.empty());
    ASSERT_EQ(f_stack.top(), 42);
}

TEST_F( StackRemoveElementsTest, Pop )
{
    for( auto it = f_input_array.crbegin();
         it != f_input_array.crend() && !f_stack.empty();
         ++it ){
        ASSERT_EQ(f_stack.top(), *it);
        f_stack.pop();
        ASSERT_TRUE(assert_invariant(f_stack));
    }
    ASSERT_TRUE(f_stack.empty());
}

} // namespace
