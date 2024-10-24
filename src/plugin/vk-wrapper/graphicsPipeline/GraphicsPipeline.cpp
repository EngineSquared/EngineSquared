/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-23
** File description:
** GraphicsPipeline
*/

#include "GraphicsPipeline.hpp"

namespace ES::Plugin::Wrapper {

void GraphicsPipeline::create(const VkDevice device)
{
    auto vertShaderCode = ShaderModule::readFile(SHADER_DIR "vert.spv");
    auto fragShaderCode = ShaderModule::readFile(SHADER_DIR "frag.spv");

    VkShaderModule vertex = ShaderModule::create(device, vertShaderCode);
    VkShaderModule fragment = ShaderModule::create(device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertex;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragment;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    ShaderModule::destroy(device, fragment);
    ShaderModule::destroy(device, vertex);
}

} // namespace ES::Plugin::Wrapper
