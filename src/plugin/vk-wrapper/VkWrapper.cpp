/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-15
** File description:
** VkWrapper
*/

#include "VkWrapper.hpp"

namespace ES::Plugin {

void VkWrapper::create(GLFWwindow *window, uint32_t width, uint32_t height, const std::string &applicationName)
{
    _instance.create(applicationName);
    _instance.setupDebugMessenger();
    _instance.createSurface(window);
    _instance.setupDevices();
    _instance.createSwapChainImages(width, height);
    _instance.createGraphicsPipeline();
    _instance.createSyncObjects();
}

void VkWrapper::destroy() { _instance.destroy(); }

Wrapper::Result VkWrapper::drawFrame() { return _instance.drawNextImage(); }

void VkWrapper::Resize(GLFWwindow *window)
{
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    _instance.recreateSwapChain(width, height);
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
    vkWrapper->setFramebufferResized();
}

} // namespace ES::Plugin
