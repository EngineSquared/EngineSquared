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
 * @file VkWrapperError.hpp
 * @brief VkWrapperError class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-11-02
 **************************************************************************/

#ifndef VKWRAPPERERROR_HPP_
#define VKWRAPPERERROR_HPP_

#include <stdexcept>
#include <string>

namespace ES::Plugin::Wrapper {

/**
 * @brief VkWrapperError is an exception class that should be thrown when an error
 * occurs while creating or destroying a Vulkan object.
 *
 * @example "Catching an exception"
 * @code
 * try {
 *    // Vulkan code
 * } catch (VkWrapperError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw VkWrapperError("Failed to create Vulkan instance");
 * @endcode
 */
class VkWrapperError : public std::exception {
  public:
    VkWrapperError(const std::string &message) : msg("VkWrapper error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  protected:
    std::string msg;
};

} // namespace ES::Plugin::Wrapper

#endif /* !VKWRAPPERERROR_HPP_ */
