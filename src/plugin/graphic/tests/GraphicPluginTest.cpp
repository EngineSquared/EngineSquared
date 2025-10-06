#include <gtest/gtest.h>

#include "Graphic.hpp"

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Plugin::Graphic::Plugin>();

    core.RunSystems();

    if (!core.GetResource<Plugin::Graphic::Resource::Context>().instance.has_value())
        GTEST_FAIL();

    GTEST_SUCCEED();
}
