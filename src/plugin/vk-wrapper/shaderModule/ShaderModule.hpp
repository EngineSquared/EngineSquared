/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * VkWrapper is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file ShaderModule.hpp
 * @brief ShaderModule class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-23
 **************************************************************************/

#ifndef SHADERMODULE_HPP_
#define SHADERMODULE_HPP_

#include <fstream>

#include "DebugMessenger.hpp"

#ifdef PROJECT_SOURCE_DIR
#    define SHADER_DIR PROJECT_SOURCE_DIR + "shaders/"
#else
#    define SHADER_DIR ""
#endif

namespace ES::Plugin::Wrapper {

/**
 * @brief ShaderModule class.
 *
 * @example
 * @code
 * auto code = ShaderModule::LoadSPVfile(SHADER_DIR"shader.spv");
 * auto module = ShaderModule::Create(device, code);
 * ShaderModule::Destroy(device, module);
 * @endcode
 */
class ShaderModule {
  public:
    /**
     * @brief ShaderPaths struct.
     *
     * This struct contains the paths to the vertex and fragment shaders.
     * The paths are stored as a pair of strings, where the first string is the path to the shader file
     * and the second string is the name of the entry point function (e.g. main).
     *
     * @note The paths are stored in the following format: {path, pName}.
     * @note The SHADER_DIR macro is used to define the directory where the shaders are stored.
     *       It is optional and can be replaced with the actual path to the shaders.
     *       It based on the PROJECT_SOURCE_DIR macro wich is defined by export.h.
     *
     * @see SHADER_DIR
     * @see PROJECT_SOURCE_DIR
     * @see export.h
     *
     * @example
     * @code
     * ShaderPaths paths = {
     *     {SHADER_DIR"vertex.spv", "main"},
     *     {SHADER_DIR"fragment.spv", "main"}
     * };
     * @endcode
     */
    struct ShaderPaths {
        std::pair<std::string /*path*/, std::string /*pName*/> vertex;
        std::pair<std::string /*path*/, std::string /*pName*/> fragment;
    };

  public:
    /**
     * @brief Loads an SPV file.
     *
     * This function loads an SPV file and returns the content as a vector of chars.
     * The function checks if the file is an SPV file and opens it in binary mode.
     * It reads the file and returns the content as a vector of chars.
     *
     * @param filename  The name of the file. The file must be an SPV file. (e.g. shader.spv)
     * @return std::vector<char>  The content of the file as a vector of chars.
     */
    static std::vector<char> LoadSPVfile(const std::string &filename);

    /**
     * @brief Creates a shader module.
     *
     * This function creates a shader module from the device and the code.
     *
     * @param device  The Vulkan device.
     * @param code  The code of the shader.
     * @return The shader module.
     */
    static VkShaderModule Create(const VkDevice &device, const std::vector<char> &code);

    /**
     * @brief Destroys a shader module.
     *
     * This function destroys a shader module.
     *
     * @param device  The Vulkan device.
     * @param shaderModule  The shader module.
     */
    static void Destroy(const VkDevice &device, const VkShaderModule &shaderModule);

    /**
     * @brief Creates a VkPipelineShaderStageCreateInfo structure for a shader stage.
     *
     * @param module The VkShaderModule containing the compiled shader code.
     * @param stage The VkShaderStageFlagBits specifying the stage of the pipeline.
     * @param pName The entry point name of the shader.
     * @return VkPipelineShaderStageCreateInfo structure initialized with the provided parameters.
     */
    static VkPipelineShaderStageCreateInfo
    CreateShaderStage(const VkShaderModule &module, const VkShaderStageFlagBits stage, const std::string &pName);
};

} // namespace ES::Plugin::Wrapper

#endif /* !SHADERMODULE_HPP_ */
