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

void LogicalDevice::createLogicalDevice(VkPhysicalDevice physicalDevice)
{
    _queueFamilies.findQueueFamilies(physicalDevice);
    auto indices = _queueFamilies.getIndices();

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
}

} // namespace ES::Plugin::Wrapper
