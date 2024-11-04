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
    int width, height;

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

    std::cout << "available extensions (" << extensionCount << "):" << std::endl;

    for (const auto &extension : extensions)
        std::cout << '\t' << extension.extensionName << std::endl;
}

void VkWrapper::PrintVersion() { std::cout << "VkWrapper version: " << VKWRAPPER_VERSION_STRING << std::endl; }

void VkWrapper::PrintConfig() { std::cout << "VkWrapper config:\n" << VKWRAPPER_CONFIG_STRING << std::endl; }

void VkWrapper::ResizeCallback(GLFWwindow *window, int width, int height)
{
    auto vkWrapper = static_cast<VkWrapper *>(glfwGetWindowUserPointer(window));
    vkWrapper->SetFramebufferResized();
}

} // namespace ES::Plugin
