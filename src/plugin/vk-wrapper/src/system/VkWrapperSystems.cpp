#include "VkWrapperSystems.hpp"

namespace ES::Plugin::Wrapper::System {

void InitVkWrapper(ES::Engine::Core &core)
{
    VkWrapper &vkWrapper = core.RegisterResource<VkWrapper>(VkWrapper());
    auto &window = core.GetResource<Window::Resource::Window>();
    int width, height;

    try
    {
        window.GetWindowSize(width, height);
        vkWrapper.CreateInstance(window.GetGLFWWindow(), "EngineSquarred", width, height);
        window.SetFramebufferSizeCallback((void *) &vkWrapper, VkWrapper::ResizeCallback);
    }
    catch (const VkWrapperError &e)
    {
        ES::Utils::Log::Error(e.what());
    }
}

void AddTextureAndModel(ES::Engine::Core &core)
{
    auto &vkWrapper = core.GetResource<VkWrapper>();
    uint32_t textureId, modelId;

    try
    {
        vkWrapper.AddTexture(ASSETS_DIR "images/texture.png", textureId);
        vkWrapper.AddModel(ASSETS_DIR "models/plan.obj", modelId);
        vkWrapper.BindTexture(textureId, modelId);
    }
    catch (const VkWrapperError &e)
    {
        ES::Utils::Log::Error(e.what());
    }
}

void AddShaders(ES::Engine::Core &core)
{
    auto &vkWrapper = core.GetResource<VkWrapper>();

    vkWrapper.AddShader(SHADER_DIR "vert.spv", "main", VkWrapper::ShaderType::VERTEX);
    vkWrapper.AddShader(SHADER_DIR "frag.spv", "main", VkWrapper::ShaderType::FRAGMENT);
}

void CreatePipeline(ES::Engine::Core &core)
{
    auto &vkWrapper = core.GetResource<VkWrapper>();

    try
    {
        vkWrapper.CreatePipeline();
    }
    catch (const VkWrapperError &e)
    {
        ES::Utils::Log::Error(e.what());
    }
}

void ChangeClearColor(ES::Engine::Core &core)
{
    core.GetResource<VkWrapper>().ChangeClearColor({0.0f, 0.0f, 0.0f, 1.0f});
}

void DisplayConfigs(ES::Engine::Core &core)
{
    VkWrapper::PrintConfig();
    VkWrapper::PrintAvailableExtensions();
}

void DrawFrame(ES::Engine::Core &core)
{
    auto &vkWrapper = core.GetResource<VkWrapper>();

    try
    {
        if (vkWrapper.DrawFrame() == Wrapper::Result::NeedResize)
        {
            auto &window = core.GetResource<Window::Resource::Window>();
            vkWrapper.Resize(window.GetGLFWWindow());
        }
    }
    catch (const VkWrapperError &e)
    {
        ES::Utils::Log::Error(e.what());
    }
}

void Destroy(ES::Engine::Core &core)
{
    auto &vkWrapper = core.GetResource<VkWrapper>();
    vkWrapper.Destroy();
}

} // namespace ES::Plugin::Wrapper::System
