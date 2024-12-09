#include <gtest/gtest.h>

#include "AssetsManager.hpp"

using namespace ES::Plugin::Object::Resource;

TEST(Registry, CreateEntity)
{
    struct TestAssets {
        int value;
    };

    AssetsManager<TestAssets> assets_manager;
    TestAssets asset{42};

    ES::Plugin::Object::Utils::AssetID assetID = assets_manager.Add(std::move(asset));

    EXPECT_EQ(assets_manager.Get(assetID).value, 42);
    EXPECT_EQ(assets_manager.Contains(assetID), true);

    assets_manager.Remove(assetID);

    EXPECT_EQ(assets_manager.Contains(assetID), false);
}
