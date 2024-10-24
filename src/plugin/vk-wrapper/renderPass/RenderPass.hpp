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
 * @file RenderPass.hpp
 * @brief RenderPass class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-24
 **************************************************************************/

#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include "DebugMessenger.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief RenderPass class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class RenderPass {
  public:
    void create(const VkDevice device, const VkFormat swapChainImageFormat);

    void destroy(const VkDevice device);

    [[nodiscard]] VkRenderPass getRenderPass() const { return _renderPass; }

  private:
    VkRenderPass _renderPass;
};

} // namespace ES::Plugin::Wrapper

#endif /* !RENDERPASS_HPP_ */
