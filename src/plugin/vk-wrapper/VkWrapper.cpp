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

VkWrapper::VkWrapper(const std::string &applicationName) : _instance(applicationName)
{
    std::cout << VKWRAPPER_CONFIG_STRING << std::endl;
    _instance.setupDebugMessenger();
    _instance.pickPhysicalDevice();
}

VkWrapper::~VkWrapper() {}

void VkWrapper::drawFrame() {}

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
