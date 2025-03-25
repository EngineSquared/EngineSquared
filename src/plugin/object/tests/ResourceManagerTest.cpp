#include <gtest/gtest.h>

#include "ResourceManager.hpp"

using namespace ES::Plugin::Object::Resource;

TEST(ResourceManagerTest, AddGetRemove)
{
    struct TestResource {
        int value;
    };

    ResourceManager<TestResource> resource_manager;
    TestResource asset{42};

    resource_manager.Add("ok", asset);

    EXPECT_EQ(resource_manager.Get("ok")->value, 42);
    EXPECT_EQ(resource_manager.Contains("ok"), true);

    resource_manager.Remove("ok");

    EXPECT_EQ(resource_manager.Contains("ok"), false);
}
