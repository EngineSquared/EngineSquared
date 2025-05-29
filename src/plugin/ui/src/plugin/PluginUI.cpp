#include "PluginUI.hpp"
#include "InitUI.hpp"
#include "RenderingPipeline.hpp"
#include "UIResource.hpp"
#include "UpdateUI.hpp"
#include "RmlShaderSystems.hpp"

void ES::Plugin::UI::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::RenderingPipeline::Plugin>();

    RegisterResource<ES::Plugin::UI::Resource::UIResource>(ES::Plugin::UI::Resource::UIResource::UIResource());

    RegisterSystems<ES::Plugin::RenderingPipeline::Init>(ES::Plugin::UI::System::Init);

    RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(
        ES::Plugin::UI::System::LoadShaderVertColor, ES::Plugin::UI::System::LoadShaderVertTexture,
        ES::Plugin::UI::System::LoadShaderVertGradient, ES::Plugin::UI::System::LoadShaderCreation,
        ES::Plugin::UI::System::LoadShaderPassthrough, ES::Plugin::UI::System::LoadShaderColorMatrix,
        ES::Plugin::UI::System::LoadShaderBlendMatrix, ES::Plugin::UI::System::LoadShaderBlur,
        ES::Plugin::UI::System::LoadShaderDropShadow, ES::Plugin::UI::System::SetupShaderVertColorUniforms,
        ES::Plugin::UI::System::SetupShaderVertTextureUniforms, ES::Plugin::UI::System::SetupShaderVertGradientUniforms,
        ES::Plugin::UI::System::SetupShaderCreationUniforms, ES::Plugin::UI::System::SetupShaderPassthroughUniform,
        ES::Plugin::UI::System::SetupShaderColorMatrix, ES::Plugin::UI::System::SetupShaderBlendMatrix,
        ES::Plugin::UI::System::SetupShaderBlur, ES::Plugin::UI::System::SetupShaderDropShadow
    );

    RegisterSystems<ES::Plugin::RenderingPipeline::RenderSetup>(ES::Plugin::UI::System::Update);
    RegisterSystems<ES::Plugin::RenderingPipeline::ToGPU>(ES::Plugin::UI::System::Render);
    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::UI::System::Destroy);
}
