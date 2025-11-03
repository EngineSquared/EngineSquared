#include <gtest/gtest.h>

#include "resource/ResourceManager.hpp"

using namespace Object::Resource;

TEST(ResourceManagerTest, AddGetSetRemove)
{
    // This struct is a basic class with no copy constructor/assignment and default move constructor/assignment because we want the default behavior to not be copyable but movable.
    struct TestResource {
        int value;
        TestResource(int v) : value(v) {}

        TestResource(const TestResource &) = delete;
        TestResource &operator=(const TestResource &) = delete;

        TestResource(TestResource &&) = default;
        TestResource &operator=(TestResource &&) = default;
    };

    ResourceManager<TestResource> resource_manager;
    TestResource asset = TestResource(42);

    entt::hashed_string id = "ok";

    auto resourceRef = resource_manager.Add(id, std::move(asset));

    EXPECT_EQ(resourceRef->value, 42);

    EXPECT_EQ(resource_manager.Get(id).value, 42);
    EXPECT_EQ(resource_manager.Contains(id), true);

    resource_manager.Get(id).value = 43;
    EXPECT_EQ(resource_manager.Get(id).value, 43);

    resource_manager.Remove(id);

    EXPECT_EQ(resource_manager.Contains(id), false);
}
