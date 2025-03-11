#include "VkWrapper.hpp"
#include "Logger.hpp"

namespace ES::Plugin {

void VkWrapper::CreateInstance(GLFWwindow *window, const std::string &applicationName, const uint32_t width,
                               const uint32_t height)
{
    _instance.Create(applicationName);
    _instance.SetupDebugMessenger();
    _instance.CreateSurface(window);
    _instance.SetupDevices();
    _instance.CreateSwapChainImages(width, height);
}

void VkWrapper::CreatePipeline()
{
    _instance.CreateGraphicsPipeline(_shaders, _textures, _models);
    _instance.CreateSyncObjects();
}

void VkWrapper::Destroy()
{
    _instance.Destroy(_textures);

    _textures.clear();
    _models.clear();
}

void VkWrapper::AddTexture(const std::string &texturePath, uint32_t &textureId)
{
    textureId = entt::hashed_string{texturePath.c_str()};

    if (!_textures.load(textureId, texturePath).second)
        throw Wrapper::VkWrapperError("failed to load texture: " + texturePath);
}

void VkWrapper::AddModel(const std::string &modelPath, uint32_t &modelId)
{
    modelId = entt::hashed_string{modelPath.c_str()};

    if (!_models.load(modelId, modelPath).second)
        throw Wrapper::VkWrapperError("failed to load model: " + modelPath);
}

void VkWrapper::AddModel(const Object::Component::Mesh &model, const std::string &modelName, uint32_t &modelId)
{
    modelId = entt::hashed_string{modelName.c_str()};

    if (!_models.load(modelId, model).second)
        throw Wrapper::VkWrapperError("failed to load model: " + modelName);
}

void VkWrapper::BindTexture(const uint32_t textureId, const uint32_t modelId)
{
    const auto &texture = _textures[textureId];
    const auto &model = _models[modelId];

    auto it = std::ranges::find(model->textures, textureId);

    if (it == model->textures.end())
        model->textures.emplace_back(textureId);
    else
        ES::Utils::Log::Warn("texture already bound to model");
}

void VkWrapper::AddShader(const std::string &shaderPath, const std::string &fname, const ShaderType &shaderType)
{
    switch (shaderType)
    {
    case ShaderType::VERTEX: _shaders.vertex = {shaderPath, fname}; break;
    case ShaderType::FRAGMENT: _shaders.fragment = {shaderPath, fname}; break;

    default: break;
    }
}

Wrapper::Result VkWrapper::DrawFrame() { return _instance.DrawNextImage(); }

void VkWrapper::Resize(GLFWwindow *window)
{
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window, &width, &height);

    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    _instance.RecreateSwapChain(width, height);
}

void VkWrapper::PrintAvailableExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::string available_extensions = fmt::format("available extensions ({}):", extensionCount);
    for (const auto &extension : extensions)
        available_extensions += "\n\t" + std::string(extension.extensionName);
    ES::Utils::Log::Info(available_extensions);
}

void VkWrapper::PrintVersion() { ES::Utils::Log::Info("VkWrapper version: " VKWRAPPER_VERSION_STRING); }

void VkWrapper::PrintConfig() { ES::Utils::Log::Info("VkWrapper config:\n" VKWRAPPER_CONFIG_STRING); }

void VkWrapper::ResizeCallback(GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
{
    auto vkWrapper = static_cast<VkWrapper *>(glfwGetWindowUserPointer(window));
    vkWrapper->SetFramebufferResized();
}

} // namespace ES::Plugin
