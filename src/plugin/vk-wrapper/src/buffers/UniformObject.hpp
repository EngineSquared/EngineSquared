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
 * @file UniformObject.hpp
 * @brief UniformObject class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-11-03
 **************************************************************************/

#ifndef UNIFORMOBJECT_HPP_
#define UNIFORMOBJECT_HPP_

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ES::Plugin::Wrapper {

/**
 * @brief UniformObject class.
 *
 * This class is used to represent a uniform object in the Vulkan API.
 * It contains the model, view, and projection matrices. The matrices are used
 * to transform the vertices in the vertex shader.
 */
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

} // namespace ES::Plugin::Wrapper

#endif /* !UNIFORMOBJECT_HPP_ */
