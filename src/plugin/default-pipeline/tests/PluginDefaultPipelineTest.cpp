#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "RenderingPipeline.hpp"
#include "Graphic.hpp"

TEST(DefaultPipeline, Casual)
{
    Engine::Core core;

    core.AddPlugins<DefaultPipeline::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RunSystems();
    SUCCEED();
}
