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
}

VkWrapper::~VkWrapper() {}

void VkWrapper::drawFrame() {}

} // namespace ES::Plugin
