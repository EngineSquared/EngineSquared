/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-23
** File description:
** ShaderModule
*/

#include "ShaderModule.hpp"

namespace ES::Plugin::Wrapper {

std::vector<char> ShaderModule::readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("failed to open file: " + filename);

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule ShaderModule::create(const VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = (const uint32_t *) code.data();

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        throw std::runtime_error("failed to create shader module!");

    return shaderModule;
}

void ShaderModule::destroy(const VkDevice device, const VkShaderModule shaderModule)
{
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

} // namespace ES::Plugin::Wrapper
