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

void Surface::create(GLFWwindow *window, const VkInstance instance)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface!");
}

void Surface::destroy(const VkInstance instance)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

} // namespace ES::Plugin::Wrapper
