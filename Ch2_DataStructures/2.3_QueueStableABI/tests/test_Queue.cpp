#include "gtest/gtest.h"
#include "Queue/Queue.h"


namespace
{


class Construction : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(f_queue.empty());
    }

    Queue<int> f_queue{};
    const Queue<int> f_const_queue{};
};

class CopyControl : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for( const auto i : f_init_array ){
            f_queue.push(i);
        }
        ASSERT_TRUE(assert_invariant);
        ASSERT_FALSE(f_queue.empty());
    }
    
    std::array<int,5> f_init_array{1,2,3,4,5};
    Queue<int> f_queue{};
};

class InsertingElements : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(f_queue.empty());
    }

    Queue<int> f_queue{};
};

class RemovingElements : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for( const auto i : f_init_array ){
            f_queue.push(i);
        }
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_FALSE(f_queue.empty());
    }

    std::array<int,5> f_init_array{1,2,3,4,5};
    Queue<int> f_queue{};
};


TEST_F( Construction, Default )
{
    ASSERT_TRUE(f_queue.empty());
}

TEST_F( CopyControl, CopyConstruction )
{
    auto copied_queue = f_queue;
    const auto const_queue = f_queue;
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_TRUE(assert_invariant(copied_queue));

    for( const auto i : f_init_array ){
        ASSERT_FALSE(copied_queue.empty());
        ASSERT_EQ(copied_queue.empty(), f_queue.empty());
        ASSERT_EQ(copied_queue.front(), i);
        ASSERT_EQ(copied_queue.front(), f_queue.front());
        copied_queue.pop();
        f_queue.pop();
    }
    ASSERT_TRUE(copied_queue.empty());
    ASSERT_TRUE(assert_invariant(copied_queue));
}

TEST_F( CopyControl, MoveConstruction )
{
    Queue<int> movedto_queue = std::move(f_queue);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_TRUE(assert_invariant(movedto_queue));
    ASSERT_TRUE(f_queue.empty());
    for( const auto i : f_init_array ){
        ASSERT_FALSE(movedto_queue.empty());
        ASSERT_EQ(movedto_queue.front(), i);
        movedto_queue.pop();
    }
    ASSERT_TRUE(assert_invariant(movedto_queue));
    ASSERT_TRUE(movedto_queue.empty());
    ASSERT_TRUE(assert_invariant(f_queue));
}

TEST_F( InsertingElements, PushLvalue )
{
    const auto value1 = 42;
    f_queue.push(value1);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), value1);
    ASSERT_EQ(f_queue.back(), value1);
    const auto value2 = 11;
    f_queue.push(value2);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), value1);
    ASSERT_EQ(f_queue.back(), value2);
}

TEST_F( InsertingElements, PushRvalue )
{
    f_queue.push(42);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), 42);
    ASSERT_EQ(f_queue.back(), 42);
    f_queue.push(11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), 42);
    ASSERT_EQ(f_queue.back(), 11);
}

TEST_F( InsertingElements, Emplace )
{
    f_queue.emplace(42);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), 42);
    ASSERT_EQ(f_queue.back(), 42);
    f_queue.emplace(11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(f_queue.front(), 42);
    ASSERT_EQ(f_queue.back(), 11);
}

TEST_F( RemovingElements, Pop )
{
    for( const auto i : f_init_array ){
        ASSERT_EQ(f_queue.front(), i);
        ASSERT_FALSE(f_queue.empty());
        f_queue.pop();
        ASSERT_TRUE(assert_invariant(f_queue));
    }
    ASSERT_TRUE(f_queue.empty());
}

} // namespace
