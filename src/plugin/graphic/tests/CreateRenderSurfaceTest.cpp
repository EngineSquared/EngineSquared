#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "exception/RenderSurfaceCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/Surface.hpp"
#include "system/initialization/CreateRenderSurface.hpp"

TEST(CreateRenderSurfaceTest, ReturnsEarlyWhenWindowSystemIsNone)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem<RenderingPipeline::Setup>(Graphic::System::CreateRenderSurface);

    EXPECT_NO_THROW(core.RunSystems());

    auto &context = core.GetResource<Graphic::Resource::Context>();
    if (context.surface.has_value())
    {
        EXPECT_FALSE(context.surface->currentTexture.has_value());
        EXPECT_FALSE(context.surface->currentTextureView.has_value());
    }
}

TEST(CreateRenderSurfaceTest, ThrowsWhenSurfaceNotCreated)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem<RenderingPipeline::Setup>([](Engine::Core &c) {
        // After CreateSurface runs (which returns early), manually create a Surface object
        // but with a null value to simulate surface not being properly created
        auto &context = c.GetResource<Graphic::Resource::Context>();
        context.surface = Graphic::Resource::Surface(std::nullopt);
        // Now change WindowSystem so CreateRenderSurface doesn't return early
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::GLFW);
    });

    core.RegisterSystem<RenderingPipeline::Setup>([](Engine::Core &c) {
        EXPECT_THROW(Graphic::System::CreateRenderSurface(c), Graphic::Exception::RenderSurfaceCreationError);
    });

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(CreateRenderSurfaceTest, SystemCanBeCalledDirectly)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    bool systemRan = false;

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem<RenderingPipeline::Setup>([&systemRan](Engine::Core &c) {
        Graphic::System::CreateRenderSurface(c);
        systemRan = true;
    });

    EXPECT_NO_THROW(core.RunSystems());
    EXPECT_TRUE(systemRan);
}
