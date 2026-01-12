#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "core/Core.hpp"
#include "plugin/PluginDefaultPipeline.hpp"

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
