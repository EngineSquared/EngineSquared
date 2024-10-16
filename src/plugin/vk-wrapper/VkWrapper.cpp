/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-15
** File description:
** VkWrapper
*/

#include "VkWrapper.hpp"

namespace ES::Plugin::VkWrapper {

VkWrapper::VkWrapper()
{
    std::cout << VKWRAPPER_CONFIG_STRING << std::endl;

    InitVulkan();
}

VkWrapper::~VkWrapper() { Cleanup(); }

void VkWrapper::InitVulkan() {}

void VkWrapper::drawFrame() {}

void VkWrapper::Cleanup() {}

} // namespace ES::Plugin::VkWrapper
