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
 * @file GraphicsPipeline.hpp
 * @brief GraphicsPipeline class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-23
 **************************************************************************/

#ifndef GRAPHICSPIPELINE_HPP_
#define GRAPHICSPIPELINE_HPP_

#include "ShaderModule.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief GraphicsPipeline class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class GraphicsPipeline {
  public:
    void create(const VkDevice device);

  private:
};

} // namespace ES::Plugin::Wrapper

#endif /* !GRAPHICSPIPELINE_HPP_ */
