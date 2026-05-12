#include <gtest/gtest.h>

#include "DynamicPlugin.hpp"

TEST(DynamicPlugin, SmokeTest)
{
    Engine::Core core;

    core.AddPlugins<DynamicPlugin::Plugin>();

    core.RunSystems();
}
