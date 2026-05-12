#include <gtest/gtest.h>

#include "DynamicPlugin.hpp"
#include "scheduler/Startup.hpp"

TEST(DynamicPlugin, MediatorTest)
{
    DynamicPlugin::Resource::LibraryMediator mediator{};
    EXPECT_TRUE(mediator.TryLoadRegistry("CasualLib", "tests/dynamicPlugins/casual/libs.json"));
    EXPECT_FALSE(mediator.TryLoadRegistry("Test1", "InexistantFile"));
    EXPECT_FALSE(mediator.TryLoadRegistry("Test2", "tests/dynamicPlugins/InvalidFile"));
    EXPECT_FALSE(mediator.TryLoadRegistry("CasualLib", "tests/dynamicPlugins/casual/libs.json"));

    EXPECT_TRUE(mediator.TryGetPluginSpecs("CasualPlugin").has_value());
    EXPECT_FALSE(mediator.TryGetPluginSpecs("InexistantPlugin").has_value());
    EXPECT_TRUE(mediator.TryGetPluginSpecs("CasualPlugin", "CasualLib").has_value());
    EXPECT_FALSE(mediator.TryGetPluginSpecs("CasualPlugin", "InexistantLib").has_value());
    EXPECT_FALSE(mediator.TryGetPluginSpecs("InexistantPlugin", "CasualLib").has_value());
    EXPECT_FALSE(mediator.TryGetPluginSpecs("InexistantPlugin", "InexistantLib").has_value());
}
