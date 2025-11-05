#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    EXPECT_NO_THROW(core.RunSystems());
}
