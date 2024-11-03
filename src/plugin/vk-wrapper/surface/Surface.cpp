/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-23
** File description:
** Surface
*/

#include "Surface.hpp"

namespace ES::Plugin::Wrapper {

void Surface::Create(GLFWwindow *window, const VkInstance &instance)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &_surface) != VK_SUCCESS)
        throw VkWrapperError("Failed to create window surface!");
}

void Surface::Destroy(const VkInstance &instance) { vkDestroySurfaceKHR(instance, _surface, nullptr); }

} // namespace ES::Plugin::Wrapper
