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
 * @file Surface.hpp
 * @brief Surface class declaration.
 *
 * The surface class is used to create a surface for the Vulkan API.
 * WSI (Window System Integration) is used to render graphics to a window or other surface.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-22
 **************************************************************************/

#ifndef SURFACE_HPP_
#define SURFACE_HPP_

#include "QueueFamilies.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Surface class.
 *
 * This class is used to create a surface for the Vulkan API. It is used to
 * render graphics to a window or other surface. The surface is created from
 * a GLFW window and the Vulkan instance.
 *
 * @example
 * @code
 * Surface surface;
 * surface.Create(window, instance);
 * surface.Destroy(instance);
 * @endcode
 */
class Surface {
  public:
    /**
     * @brief Creates a surface for the Vulkan API.
     *
     * This function creates a surface for the Vulkan API using a GLFW window
     * and the Vulkan instance. The surface is used to render graphics to the
     * window or other surface.
     *
     * @param window  The GLFW window.
     * @param instance  The Vulkan instance.
     */
    void Create(GLFWwindow *window, const VkInstance &instance);

    /**
     * @brief Destroys the surface.
     *
     * This function destroys the surface created for the Vulkan API.
     *
     * @param instance  The Vulkan instance.
     */
    void Destroy(const VkInstance &instance);

    /**
     * @brief Returns the surface.
     *
     * This function returns the surface created for the Vulkan API.
     *
     * @return The surface.
     */
    [[nodiscard]] const VkSurfaceKHR &Get() const { return _surface; }

  private:
    VkSurfaceKHR _surface;
};

} // namespace ES::Plugin::Wrapper

#endif /* !SURFACE_HPP_ */
