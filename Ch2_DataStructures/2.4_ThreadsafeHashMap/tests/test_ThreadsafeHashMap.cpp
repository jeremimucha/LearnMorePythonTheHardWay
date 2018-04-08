#include "gtest/gtest.h"
#include "ThreadsafeHashMap/ThreadsafeHashMap.h"
#include <string>


namespace
{


class ThreadsafeHashMapTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        f_map.add_or_update_mapping(1, "one");
        f_map.add_or_update_mapping(2, "two");
        f_map.add_or_update_mapping(3, "three");
    }
    const std::string defstring{"default"};
    ThreadsafeHashMap<int,std::string> f_map{};
};


TEST_F(ThreadsafeHashMapTest, Dummy)
{
    ASSERT_TRUE(true);
}

TEST_F(ThreadsafeHashMapTest, AddMappingTest )
{
    const std::string four{"four"};
    ASSERT_EQ(f_map.value_for(4, defstring), defstring);
    f_map.add_or_update_mapping(4, four);
    ASSERT_EQ(f_map.value_for(4, defstring), four);
}

TEST_F(ThreadsafeHashMapTest, UpdateMappingTest)
{
    const std::string three{"three"};
    const std::string THREE{"THREE"};
    ASSERT_EQ(f_map.value_for(3,defstring), three);
    f_map.add_or_update_mapping(3,THREE);
    ASSERT_EQ(f_map.value_for(3,defstring), THREE);
}

TEST_F(ThreadsafeHashMapTest, RemoveMappingTest)
{
    const std::string three{"three"};
    ASSERT_EQ(f_map.value_for(3,defstring), three);
    f_map.remove_mapping(3);
    ASSERT_EQ(f_map.value_for(3,defstring), defstring);
}

} // namespace
