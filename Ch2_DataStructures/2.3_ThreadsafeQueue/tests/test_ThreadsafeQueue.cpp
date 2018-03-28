#include "gtest/gtest.h"
#include "ThreadsafeQueue/ThreadsafeQueue.h"


namespace
{

class ThreadsafeQueueTests : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(f_queue.empty());
    }

    ThreadsafeQueue<int> f_queue{};
};


TEST_F( ThreadsafeQueueTests, Constructor )
{
    ASSERT_TRUE(f_queue.empty());
}

} // namespace
