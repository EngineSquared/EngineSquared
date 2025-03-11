/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the Engine² project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * Engine² is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file VkWrapperSystem.hpp
 * @brief VkWrapper system declaration.
 *
 * This file contains the declaration of the VkWrapper systems.
 * The VkWrapper systems are used to create and manage the VkWrapper.
 * All VkWrapper systems are automatically managed by the Engine² Core.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2025-03-11
 **************************************************************************/

#ifndef VKWRAPPERSYSTEMS_HPP_
#define VKWRAPPERSYSTEMS_HPP_

#include "Engine.hpp"
#include "VkWrapper.hpp"
#include "Window.hpp"

#define ASSETS_DIR PROJECT_SOURCE_DIR + "assets/"

namespace ES::Plugin::Wrapper::System {

/**
 * @brief Initialize the VkWrapper.
 *
 * @param core  The Engine² Core.
 */
void InitVkWrapper(ES::Engine::Core &core);

/**
 * @brief Add a texture and a model to the VkWrapper.
 *
 * @param core  The Engine² Core.
 */
void AddTextureAndModel(ES::Engine::Core &core);

/**
 * @brief Add the shaders to the VkWrapper.
 *
 * @param core  The Engine² Core.
 */
void AddShaders(ES::Engine::Core &core);

/**
 * @brief Create the graphics pipeline.
 *
 * @param core  The Engine² Core.
 */
void CreatePipeline(ES::Engine::Core &core);

/**
 * @brief Change the clear color of the VkWrapper.
 *
 * @param core  The Engine² Core.
 */
void ChangeClearColor(ES::Engine::Core &core);

/**
 * @brief Display the available extensions for the Vulkan API.
 *
 * @param core  The Engine² Core.
 */
void DisplayConfigs(ES::Engine::Core &core);

/**
 * @brief Draw a frame.
 *
 * @param core  The Engine² Core.
 */
void DrawFrame(ES::Engine::Core &core);

/**
 * @brief Destroy the VkWrapper.
 *
 * @param core  The Engine² Core.
 */
void Destroy(ES::Engine::Core &core);

} // namespace ES::Plugin::Wrapper::System

#endif /* !VKWRAPPERSYSTEMS_HPP_ */
