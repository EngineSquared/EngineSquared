#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Plugin::Graphic::Plugin>();

    core.RegisterSystem<Plugin::RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Plugin::Graphic::Resource::GraphicSettings>().SetWindowSystem(
            Plugin::Graphic::Resource::WindowSystem::None);
    });

    EXPECT_NO_THROW(core.RunSystems());
}
