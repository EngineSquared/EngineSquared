#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"


void TextureTest(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    
    auto texture = Graphic::Resource::Texture::Create(context, Graphic::Resource::FileTextureDescriptor("textureName")
                                                        .LoadFile(std::filesystem::current_path().string() + "/assets/test_texture.png"));

}

TEST(Texture, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(TextureTest);

    EXPECT_NO_THROW(core.RunSystems());
}
