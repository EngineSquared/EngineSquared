/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-22
** File description:
** LogicalDevice
*/

#include "LogicalDevice.hpp"

namespace ES::Plugin::Wrapper {

void LogicalDevice::create(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    _queueFamilies.findQueueFamilies(physicalDevice, surface);
    auto indices = _queueFamilies.getIndices();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = (uint32_t) queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = (uint32_t) deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
        throw std::runtime_error("Failed to create logical device!");

    vkGetDeviceQueue(_device, indices.graphicsFamily.value(), 0, &_graphicsQueue);
    vkGetDeviceQueue(_device, indices.presentFamily.value(), 0, &_presentQueue);
}

} // namespace ES::Plugin::Wrapper
