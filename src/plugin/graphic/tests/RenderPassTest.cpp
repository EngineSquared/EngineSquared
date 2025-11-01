#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "utils/CreateDefaultTestShader.hpp"

void TestSystem(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    Graphic::Resource::SingleExecutionRenderPass renderPass("TestRenderPass");

    std::shared_ptr<Graphic::Resource::Shader> shader =
        std::make_shared<Graphic::Resource::Shader>(Graphic::Tests::Utils::CreateDefaultTestShader(context));

    renderPass.BindShader(shader);
    renderPass.SetGetClearColorCallback([](Engine::Core &core, glm::vec4 &color) {
        color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return true;
    });
    renderPass.AddInput(0, "TestInput");

    EXPECT_NO_THROW(renderPass.Execute(core));
}

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    EXPECT_NO_THROW(core.RunSystems());
}
