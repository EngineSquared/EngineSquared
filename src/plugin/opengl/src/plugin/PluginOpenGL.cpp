#include "OpenGL.pch.hpp"

#include "Input.hpp"
#include "plugin/PluginWindow.hpp"
#include "RenderingPipeline.hpp"
#include "scheduler/Startup.hpp"
#include "scheduler/Update.hpp"

#include "OpenGL.hpp"

void ES::Plugin::OpenGL::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::RenderingPipeline::Plugin, ES::Plugin::Window::Plugin, ES::Plugin::Input::Plugin>();

    RegisterSystems<ES::Plugin::RenderingPipeline::Init>(ES::Plugin::OpenGL::System::InitGLEW,
                                                         ES::Plugin::OpenGL::System::CheckGLEWVersion);

    RegisterResource<ES::Plugin::OpenGL::Resource::DirectionalLight>(ES::Plugin::OpenGL::Resource::DirectionalLight{});

    RegisterSystems<ES::Plugin::RenderingPipeline::Setup>(
        ES::Plugin::OpenGL::System::GlEnableMultiSample, ES::Plugin::OpenGL::System::SetupResizeViewport,
        ES::Plugin::OpenGL::System::LoadFontManager, ES::Plugin::OpenGL::System::LoadMaterialCache,
        ES::Plugin::OpenGL::System::LoadShaderManager, ES::Plugin::OpenGL::System::LoadDefaultShader,
        ES::Plugin::OpenGL::System::LoadDefaultTextShader, ES::Plugin::OpenGL::System::LoadDefaultSpriteShader,
        ES::Plugin::OpenGL::System::LoadNoTextureLightShadowShader, ES::Plugin::OpenGL::System::LoadDepthMapShader,
        ES::Plugin::OpenGL::System::LoadTextureManager, ES::Plugin::OpenGL::System::LoadCubeMapManager,
        ES::Plugin::OpenGL::System::CreateCamera, ES::Plugin::OpenGL::System::SetupShaderUniforms,
        ES::Plugin::OpenGL::System::SetupTextShaderUniforms, ES::Plugin::OpenGL::System::SetupSpriteShaderUniforms,
        ES::Plugin::OpenGL::System::SetupNoTextureLightShadowShader, ES::Plugin::OpenGL::System::SetupDepthMapShader,
        ES::Plugin::OpenGL::System::LoadGLMeshBufferManager, ES::Plugin::OpenGL::System::LoadGLTextBufferManager,
        ES::Plugin::OpenGL::System::LoadGLSpriteBufferManager, ES::Plugin::OpenGL::System::LoadDefaultSkyBoxShader,
        ES::Plugin::OpenGL::System::SetupSkyBoxhMapShader, ES::Plugin::OpenGL::System::SetupMouseDragging,
        ES::Plugin::OpenGL::System::GenerateDirectionalLightFramebuffer,
        ES::Plugin::OpenGL::System::GenerateDirectionalLightTexture,
        ES::Plugin::OpenGL::System::BindDirectionalLightTextureToFramebuffer);

    RegisterSystems<ES::Plugin::RenderingPipeline::RenderSetup>(
        ES::Plugin::OpenGL::System::GLClearColor, ES::Plugin::OpenGL::System::GLClearDepth,
        ES::Plugin::OpenGL::System::GLEnableDepth, ES::Plugin::OpenGL::System::GLEnableCullFace,
        ES::Plugin::OpenGL::System::UpdateMatrices, ES::Plugin::OpenGL::System::SetupCamera,
        ES::Plugin::OpenGL::System::SetupLights, ES::Plugin::OpenGL::System::LoadGLMeshBuffer,
        ES::Plugin::OpenGL::System::UpdateNoTextureLightShadowShader, ES::Plugin::OpenGL::System::UpdateDepthMapShader,
        ES::Plugin::OpenGL::System::LoadGLTextBuffer, ES::Plugin::OpenGL::System::LoadGLSpriteBuffer);

    RegisterSystems<ES::Plugin::RenderingPipeline::ToGPU>(
        ES::Plugin::OpenGL::System::SetupShadowframebuffer, ES::Plugin::OpenGL::System::RenderShadowMap,
        ES::Plugin::OpenGL::System::ResetPassStatus, ES::Plugin::OpenGL::System::RenderMeshes,
        ES::Plugin::OpenGL::System::RenderText, ES::Plugin::OpenGL::System::RenderSprites,
        ES::Plugin::OpenGL::System::GLEnableDepthLEqual, ES::Plugin::OpenGL::System::GLDisableDepthWrite,
        ES::Plugin::OpenGL::System::RenderSkyBox, ES::Plugin::OpenGL::System::GLResetDepthFunc,
        ES::Plugin::OpenGL::System::GLEnableDepthWrite);
}
