#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

void TextureTest(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    std::string testAssetPath = std::filesystem::current_path().string() + "/assets/test_texture.png";

    auto image = Graphic::Resource::Image::LoadFromFile(testAssetPath);

    auto texture = Graphic::Resource::Texture::Create(context, testAssetPath, image);

    auto data = texture.GetDataTexture(context);

    EXPECT_EQ(data.width, image.width);
    EXPECT_EQ(data.height, image.height);

    for (uint32_t y = 0; y < data.height; ++y)
    {
        for (uint32_t x = 0; x < data.width; ++x)
        {
            size_t index = y * data.width + x;
            EXPECT_EQ(data.pixels[index], image.pixels[index]);
        }
    }
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
