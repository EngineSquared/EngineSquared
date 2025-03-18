#include <gtest/gtest.h>

#include "AssetsManager.hpp"

using namespace ES::Plugin::Object::Resource;

TEST(Core, CreateEntity)
{
    EXPECT_EQ(1, 2);

    struct TestAssets {
        int value;
    };

    AssetsManager<TestAssets> assets_manager;
    TestAssets asset{42};

    assets_manager.Add(1, std::move(asset));

    EXPECT_EQ(assets_manager.Get(1).value, 42);
    EXPECT_EQ(assets_manager.Contains(1), true);

    assets_manager.Remove(1);

    EXPECT_EQ(assets_manager.Contains(1), false);
}
