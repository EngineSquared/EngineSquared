#include <gtest/gtest.h>

#include "ResourceManager.hpp"

using namespace ES::Plugin::Object::Resource;

TEST(ResourceManagerTest, AddGetSetRemove)
{
    struct TestResource {
        int value;
    };

    ResourceManager<TestResource> resource_manager;
    TestResource asset{42};

    entt::hashed_string id = "ok";

    resource_manager.Add(id, asset);

    EXPECT_EQ(resource_manager.Get(id).value, 42);
    EXPECT_EQ(resource_manager.Contains(id), true);

    resource_manager.Get(id).value = 43;
    EXPECT_EQ(resource_manager.Get(id).value, 43);

    resource_manager.Remove(id);

    EXPECT_EQ(resource_manager.Contains(id), false);
}
