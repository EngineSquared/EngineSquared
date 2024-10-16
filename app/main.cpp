/*
** EPITECH PROJECT, 2024
** EngineSquared
** File description:
** main
*/

#define GLFW_INCLUDE_VULKAN
#include "Window.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <iostream>

int main(int ac, char **av)
{
    ES::Plugin::Window::Resource::Window window(800, 600, "My Engine");

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!window.ShouldClose())
    {
        glfwPollEvents();
    }

    return 0;
}
