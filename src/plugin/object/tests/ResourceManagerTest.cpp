#include <gtest/gtest.h>

#include "resource/ResourceManager.hpp"

using namespace Object::Resource;

TEST(ResourceManagerTest, AddGetSetRemove)
{
    // This struct is a basic class with no copy constructor/assignment and default move constructor/assignment because
    // we want the default behavior to not be copyable but movable.
    struct TestResource {
        int value;
        explicit TestResource(int v) : value(v) {}

        TestResource(const TestResource &) = delete;
        TestResource &operator=(const TestResource &) = delete;

        TestResource(TestResource &&) = default;
        TestResource &operator=(TestResource &&) = default;
    };

    ResourceManager<TestResource> resource_manager;
    auto asset = TestResource(42);

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

TEST(ResourceManagerTest, SetDefaultAndGetOrDefault)
{
    struct TestResource {
        int value;
        explicit TestResource(int v) : value(v) {}

        TestResource(const TestResource &) = delete;
        TestResource &operator=(const TestResource &) = delete;

        TestResource(TestResource &&) = default;
        TestResource &operator=(TestResource &&) = default;
    };

    ResourceManager<TestResource> resource_manager;

    entt::hashed_string resourceId = "resource";
    entt::hashed_string missingId = "missing";

    // Add regular resource
    resource_manager.Add(resourceId, TestResource(200));

    // Check that no default is set initially
    EXPECT_FALSE(resource_manager.HasDefault());

    // Set default resource
    resource_manager.SetDefault(TestResource(100));
    EXPECT_TRUE(resource_manager.HasDefault());

    // GetOrDefault should return the existing resource
    EXPECT_EQ(resource_manager.GetOrDefault(resourceId).value, 200);

    // GetOrDefault should return the default resource when the requested one doesn't exist
    EXPECT_EQ(resource_manager.GetOrDefault(missingId).value, 100);

    // Regular Get should still work as before
    EXPECT_EQ(resource_manager.Get(resourceId).value, 200);
}


TEST(ResourceManagerTest, GetOrDefaultWithoutDefaultSet)
{
    struct TestResource {
        int value;
        explicit TestResource(int v) : value(v) {}

        TestResource(const TestResource &) = delete;
        TestResource &operator=(const TestResource &) = delete;

        TestResource(TestResource &&) = default;
        TestResource &operator=(TestResource &&) = default;
    };

    ResourceManager<TestResource> resource_manager;

    entt::hashed_string missingId = "missing";

    // GetOrDefault without a default set should throw
    EXPECT_THROW((void)resource_manager.GetOrDefault(missingId), Object::ResourceManagerError);
}

TEST(ResourceManagerTest, GetOrDefaultConst)
{
    struct TestResource {
        int value;
        explicit TestResource(int v) : value(v) {}

        TestResource(const TestResource &) = delete;
        TestResource &operator=(const TestResource &) = delete;

        TestResource(TestResource &&) = default;
        TestResource &operator=(TestResource &&) = default;
    };

    ResourceManager<TestResource> resource_manager;

    entt::hashed_string missingId = "missing";

    resource_manager.SetDefault(TestResource(100));

    // Test const version of GetOrDefault
    const auto &constManager = resource_manager;
    EXPECT_EQ(constManager.GetOrDefault(missingId).value, 100);
}
