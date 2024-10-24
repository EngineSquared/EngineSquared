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

VkWrapper::VkWrapper(GLFWwindow *window, uint32_t width, uint32_t height, const std::string &applicationName)
    : _instance(applicationName)
{
    std::cout << VKWRAPPER_CONFIG_STRING << std::endl;
    _instance.setupDebugMessenger();
    _instance.createSurface(window);
    _instance.setupDevices();
    _instance.createSwapChainImages(width, height);
    _instance.createGraphicsPipeline();
    _instance.createSemaphores();
}

void VkWrapper::drawFrame()
{
    uint32_t imageIndex;
    _instance.acquireNextImage(imageIndex);
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

} // namespace ES::Plugin
