#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "exception/EndRenderTextureCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/Surface.hpp"
#include "resource/TextureContainer.hpp"
#include "system/preparation/CreateEndRenderTexture.hpp"

using Graphic::System::END_RENDER_TEXTURE_ID;

TEST(CreateEndRenderTextureTest, CreatesTextureWhenWindowSystemIsNone)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem([](Engine::Core &core) {
        auto const &context = core.GetResource<Graphic::Resource::Context>();
        auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();

        EXPECT_FALSE(context.surface.has_value());
        EXPECT_TRUE(textureContainer.Contains(END_RENDER_TEXTURE_ID));
    });

    EXPECT_NO_THROW(core.RunSystems());
}
