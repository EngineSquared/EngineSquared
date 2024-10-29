/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @MasterLaplace, All rights reserved.
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
#include <string>

#include "DebugMessenger.hpp"

#define SHADER_DIR PROJECT_SOURCE_DIR + "shaders/"

namespace ES::Plugin::Wrapper {

/**
 * @brief ShaderModule class.
 *
 * @example
 * @code
 * auto code = ShaderModule::readFile(SHADER_DIR"shader.spv");
 * auto module = ShaderModule::create(device, code);
 * ShaderModule::destroy(device, module);
 * @endcode
 */
class ShaderModule {
  public:
    /**
     * @brief Reads a file.
     *
     * This function reads a file and returns the content as a vector of chars.
     *
     * @param filename  The name of the file.
     * @return The content of the file as a vector of chars.
     */
    static std::vector<char> readFile(const std::string &filename);

    /**
     * @brief Creates a shader module.
     *
     * This function creates a shader module from the device and the code.
     *
     * @param device  The Vulkan device.
     * @param code  The code of the shader.
     * @return The shader module.
     */
    static VkShaderModule create(const VkDevice device, const std::vector<char> &code);

    /**
     * @brief Destroys a shader module.
     *
     * This function destroys a shader module.
     *
     * @param device  The Vulkan device.
     * @param shaderModule  The shader module.
     */
    static void destroy(const VkDevice device, const VkShaderModule shaderModule);
};

} // namespace ES::Plugin::Wrapper

#endif /* !SHADERMODULE_HPP_ */
