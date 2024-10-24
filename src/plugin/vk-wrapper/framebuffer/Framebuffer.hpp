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
 * @file Framebuffer.hpp
 * @brief Framebuffer class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-24
 **************************************************************************/

#ifndef FRAMEBUFFER_HPP_
#define FRAMEBUFFER_HPP_

#include "DebugMessenger.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Framebuffer class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class Framebuffer {
  public:
    void create(const VkDevice device, const VkExtent2D swapChainExtent, const VkRenderPass renderPass,
                const std::vector<VkImageView> &swapChainImageViews);

    void destroy(const VkDevice device);

  private:
    std::vector<VkFramebuffer> _swapChainFramebuffers;
};

} // namespace ES::Plugin::Wrapper

#endif /* !FRAMEBUFFER_HPP_ */
