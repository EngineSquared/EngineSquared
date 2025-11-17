#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "utils/CreateDefaultTestShader.hpp"

void TestSystem(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();

    const entt::hashed_string shaderId = "DefaultTestShader";

    auto shader = Graphic::Tests::Utils::CreateDefaultTestShader(context);

    core.GetResource<Graphic::Resource::ShaderContainer>().Add(shaderId, std::move(shader));

    Graphic::Resource::SingleExecutionRenderPass renderPass("TestRenderPass");

    renderPass.BindShader(shaderId);
    renderPass.SetGetClearColorCallback([](Engine::Core &core, glm::vec4 &color) {
        color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return true;
    });
    renderPass.AddInput(0, "TestInput");

    EXPECT_NO_THROW(renderPass.Execute(core));

    auto validationErrors = renderPass.validate(core);

    if (!validationErrors.empty())
    {
        for (const auto &error : validationErrors)
        {
            std::cout << error << std::endl;
        }
        FAIL() << "RenderPass validation failed with errors.";
    }
}

void TestShaderNotFoundValidation(Engine::Core &core)
{
    const entt::hashed_string nonExistentShaderId = "NonExistentShader";

    Graphic::Resource::SingleExecutionRenderPass renderPass("TestRenderPassWithMissingShader");

    renderPass.BindShader(nonExistentShaderId);
    renderPass.SetGetClearColorCallback([](Engine::Core &core, glm::vec4 &color) {
        color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return true;
    });

    auto validationErrors = renderPass.validate(core);

    // Should have at least one error for missing shader
    EXPECT_FALSE(validationErrors.empty());

    // Check that we have a specific error about the shader not being found
    bool foundShaderError = false;
    for (const auto &error : validationErrors)
    {
        if (error.message.find("not found in resource manager") != std::string::npos)
        {
            foundShaderError = true;
            EXPECT_EQ(error.severity, Graphic::Utils::ValidationError::Severity::Error);
        }
    }
    EXPECT_TRUE(foundShaderError) << "Expected validation error for missing shader";
}

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(TestSystem);

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(GraphicPlugin, RenderPassShaderValidation)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(TestShaderNotFoundValidation);

    EXPECT_NO_THROW(core.RunSystems());
}
