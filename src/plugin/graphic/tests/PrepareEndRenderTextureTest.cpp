#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

TEST(PrepareEndRenderTextureTest, CreatesTextureWhenWindowSystemIsNone)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem<RenderingPipeline::Extraction>([](Engine::Core &core) {
        auto const &context = core.GetResource<Graphic::Resource::Context>();
        auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

        EXPECT_FALSE(context.surface.has_value());
        EXPECT_TRUE(textureContainer.Contains(Graphic::Utils::END_RENDER_TEXTURE_ID));
    });

    EXPECT_NO_THROW(core.RunSystems());
}
