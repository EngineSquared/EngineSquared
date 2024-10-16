/*
** EPITECH PROJECT, 2024
** EngineSquared
** File description:
** main
*/

#include "VkWrapper.hpp"
#include "Window.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include <iostream>

int main(int ac, char **av)
{
    ES::Plugin::Window::Resource::Window window(800, 600, "My Engine");
    ES::Plugin::VkWrapper vkWrapper("My Engine");

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    try
    {
        while (!window.ShouldClose())
        {
            glfwPollEvents();
            vkWrapper.drawFrame();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
