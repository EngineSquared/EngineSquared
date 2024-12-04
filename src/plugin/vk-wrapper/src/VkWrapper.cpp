#include "Logger.hpp"

#include "VkWrapper.hpp"

namespace ES::Plugin {

void VkWrapper::Create(const CreateInfo &info)
{
    _instance.Create(info.applicationName);
    _instance.SetupDebugMessenger();
    _instance.CreateSurface(info.window);
    _instance.SetupDevices();
    _instance.CreateSwapChainImages(info.width, info.height);
    _instance.CreateGraphicsPipeline(info.shaders);
    _instance.CreateSyncObjects();
}

void VkWrapper::Destroy() { _instance.Destroy(); }

Wrapper::Result VkWrapper::DrawFrame() { return _instance.DrawNextImage(); }

void VkWrapper::Resize(GLFWwindow *window)
{
    int width;
    int height;

    glfwGetFramebufferSize(window, &width, &height);

    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    _instance.RecreateSwapChain(width, height);
}

void VkWrapper::PrintAvailableExtensions() const
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::string available_extensions = fmt::format("available extensions ({}):", extensionCount);
    for (const auto &extension : extensions)
        available_extensions += "\t" + std::string(extension.extensionName);
    ES::Utils::Log::Info(available_extensions);
}

void VkWrapper::PrintVersion() const { ES::Utils::Log::Info("VkWrapper version: " VKWRAPPER_VERSION_STRING); }

void VkWrapper::PrintConfig() const { ES::Utils::Log::Info("VkWrapper config:\n" VKWRAPPER_CONFIG_STRING); }

void VkWrapper::ResizeCallback(GLFWwindow *window, [[maybe_unused]] int width, [[maybe_unused]] int height)
{
    auto vkWrapper = static_cast<VkWrapper *>(glfwGetWindowUserPointer(window));
    vkWrapper->SetFramebufferResized();
}

} // namespace ES::Plugin
