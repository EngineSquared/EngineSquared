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
 * @file Instance.hpp
 * @brief Instance class declaration.
 *
 * This class is used to create an instance of the Vulkan API.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

#ifndef INSTANCE_HPP_
#define INSTANCE_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace ES::Plugin::Wrapper {

/**
 * @brief Instance class.
 *
 * This class is used to create an instance of the Vulkan API.
 *
 * @example
 * @code
 * Instance instance("My Engine");
 * @endcode
 */
class Instance {
  public:
    Instance(const std::string &applicationName);
    ~Instance();

  private:
    VkInstance instance;
};

} // namespace ES::Plugin::Wrapper

#endif /* !INSTANCE_HPP_ */
