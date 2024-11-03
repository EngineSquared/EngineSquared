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

std::vector<char> ShaderModule::loadSPVfile(const std::string &filename)
{
    if (filename.substr(filename.find_last_of(".") + 1) != "spv")
        throw VkWrapperError("file is not an spv file: " + filename);

    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
        throw VkWrapperError("failed to open file: " + filename);

    size_t fileSize = static_cast<size_t>(file.tellg());
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
        throw VkWrapperError("failed to create shader module!");

    return shaderModule;
}

void ShaderModule::destroy(const VkDevice device, const VkShaderModule shaderModule)
{
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

VkPipelineShaderStageCreateInfo ShaderModule::createShaderStage(const VkShaderModule &module,
                                                                const VkShaderStageFlagBits &stage,
                                                                const std::string &pName)
{
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = stage;
    shaderStageInfo.module = module;
    shaderStageInfo.pName = pName.c_str();
    return shaderStageInfo;
}

} // namespace ES::Plugin::Wrapper
