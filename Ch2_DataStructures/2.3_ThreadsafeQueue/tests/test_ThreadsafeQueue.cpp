#include "gtest/gtest.h"
#include "ThreadsafeQueue/ThreadsafeQueue.h"
#include <array>
#include <future>
#include <random>
#include <chrono>


namespace
{

class AddElementsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(f_queue.empty());
    }

    ThreadsafeQueue<int> f_queue{};
};

class RemoveElementsTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        for(auto i : f_init_array)
            f_queue.push(i);
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_FALSE(f_queue.empty());
    }

    std::array<int,5>    f_init_array{1,2,3,4,5};
    ThreadsafeQueue<int> f_queue{};
};


class ConcurrentTest : public ::testing::Test
{
};

template<typename Function>
class Periodic_action
{
public:
    explicit Periodic_action(Function f, int min, int max)
        : action(f), ud(min, max) { }

    void operator()(int iterations=0)
    {
        for(int i=0; i<iterations; ++i){
            action();
            std::this_thread::sleep_for(std::chrono::milliseconds(ud(re)));
        }
    }
private:
    static std::default_random_engine re;
    Function action;
    std::uniform_int_distribution<> ud;
};
template<typename T>
std::default_random_engine Periodic_action<T>::re{
    static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())};


TEST_F( AddElementsTest, Constructor )
{
    ASSERT_TRUE(f_queue.empty());
}

TEST_F( AddElementsTest, PushLvalue )
{
    constexpr auto value1 = 42;
    constexpr auto value2 = 11;
    f_queue.push(value1);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    f_queue.push(value2);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(*f_queue.try_pop(),value1);
    ASSERT_EQ(*f_queue.try_pop(),value2);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_TRUE(f_queue.empty());
}

TEST_F( AddElementsTest, PushRvalue )
{
    f_queue.push(42);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    f_queue.push(11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(*f_queue.try_pop(),42);
    ASSERT_EQ(*f_queue.try_pop(),11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_TRUE(f_queue.empty());
}

TEST_F( AddElementsTest, Emplace )
{
    f_queue.emplace(42);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    f_queue.emplace(11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_FALSE(f_queue.empty());
    ASSERT_EQ(*f_queue.try_pop(),42);
    ASSERT_EQ(*f_queue.try_pop(),11);
    ASSERT_TRUE(assert_invariant(f_queue));
    ASSERT_TRUE(f_queue.empty());
}

TEST_F( RemoveElementsTest, TryPopSharedPtr )
{
    for( auto i : f_init_array ){
        ASSERT_FALSE(f_queue.empty());
        const auto target = f_queue.try_pop();
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(target != nullptr);
        ASSERT_EQ(i, *target);
    }
    ASSERT_TRUE(f_queue.empty());
    const auto target = f_queue.try_pop();
    ASSERT_TRUE(target == nullptr);
    ASSERT_TRUE(assert_invariant(f_queue));
}

TEST_F( RemoveElementsTest, TryPopParam )
{
    auto value{0};
    for( auto i : f_init_array ){
        ASSERT_FALSE(f_queue.empty());
        ASSERT_TRUE(f_queue.try_pop(value));
        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_EQ(i, value);
    }
    ASSERT_TRUE(f_queue.empty());
    ASSERT_FALSE(f_queue.try_pop(value));
    ASSERT_TRUE(assert_invariant(f_queue));
}

TEST_F( ConcurrentTest, WaitAndPopSharedPtr )
{
    std::promise<void> go, push_ready, pop_ready;
    std::shared_future<void> ready{go.get_future()};
    ThreadsafeQueue<int> f_queue{};
    try{
        auto push_done = std::async(std::launch::async,
                            [&f_queue, &push_ready, ready=ready]()
                            {
                                push_ready.set_value();
                                ready.wait();
                                auto push_action = [&f_queue](){f_queue.push(42);};
                                Periodic_action<decltype(push_action)>{push_action,100,100}(10);
                            });

        auto pop_done = std::async(std::launch::async,
                            [&f_queue, &pop_ready, ready=ready]()
                            {
                                pop_ready.set_value();
                                ready.wait();
                                auto pop_action = [&f_queue]()
                                {
                                    auto value{0};
                                    f_queue.wait_and_pop(value);
                                    ASSERT_EQ(value,42);
                                };
                                Periodic_action<decltype(pop_action)>{pop_action,50,200}(10);
                            });

        push_ready.get_future().wait();
        pop_ready.get_future().wait();
        go.set_value();

        ASSERT_TRUE(assert_invariant(f_queue));
        ASSERT_TRUE(f_queue.empty());
    }
    catch(...){
        go.set_value();
        throw;
    }
}

// --- Not testable with singlethreaded code
// TEST_F( RemoveElementsTest, WaitAndPopSharedPtr )
// {
//     for( auto i : f_init_array ){
//         ASSERT_FALSE(f_queue.empty());
//         const auto target = f_queue.wait_and_pop();
//         ASSERT_TRUE(assert_invariant(f_queue));
//         ASSERT_TRUE(target != nullptr);
//         ASSERT_EQ(i, *target);
//     }
//     ASSERT_TRUE(f_queue.empty());
//     const auto target = f_queue.wait_and_pop();
//     ASSERT_TRUE(target == nullptr);
//     ASSERT_TRUE(assert_invariant(f_queue));
// }

// TEST_F( RemoveElementsTest, WaitAndPopParam )
// {
//     auto value{0};
//     for( auto i : f_init_array ){
//         ASSERT_FALSE(f_queue.empty());
//         f_queue.wait_and_pop(value);
//         ASSERT_TRUE(assert_invariant(f_queue));
//         ASSERT_EQ(i, value);
//     }
//     ASSERT_TRUE(f_queue.empty());
//     ASSERT_TRUE(assert_invariant(f_queue));
// }


} // namespace
