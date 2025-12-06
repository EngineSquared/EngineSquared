#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "exception/RenderSurfaceCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/Surface.hpp"
#include "resource/TextureContainer.hpp"
#include "system/initialization/CreateRenderSurface.hpp"

TEST(CreateRenderSurfaceTest, CreatesTextureWhenWindowSystemIsNone)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    EXPECT_NO_THROW(core.RunSystems());

    auto const &context = core.GetResource<Graphic::Resource::Context>();
    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    entt::hashed_string textureId = "surface_current_texture";

    EXPECT_FALSE(context.surface.has_value());
    EXPECT_TRUE(textureContainer.Contains(textureId));
}

TEST(CreateRenderSurfaceTest, ThrowsWhenSurfaceNotCreated)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem<RenderingPipeline::Setup>([](Engine::Core &c) {
        // After CreateSurface runs (which returns early when WindowSystem is None),
        // manually create a Surface object but with a null value to simulate surface not being properly created
        auto &context = c.GetResource<Graphic::Resource::Context>();
        context.surface = Graphic::Resource::Surface(std::nullopt);
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::GLFW);
    });

    core.RegisterSystem<RenderingPipeline::Setup>([](Engine::Core &c) {
        EXPECT_THROW(Graphic::System::CreateRenderSurface(c), Graphic::Exception::RenderSurfaceCreationError);
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(CreateRenderSurfaceTest, CreatesTextureInContainerWhenWindowSystemIsNone)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    EXPECT_NO_THROW(core.RunSystems());

    auto &textureContainer = core.GetResource<Graphic::Resource::TextureContainer>();
    entt::hashed_string textureId = "surface_current_texture";
    
    EXPECT_TRUE(textureContainer.Contains(textureId));
    EXPECT_NO_THROW(auto &texture = textureContainer.Get(textureId); (void)texture;);
}
