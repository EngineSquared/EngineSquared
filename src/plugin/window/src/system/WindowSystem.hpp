/**************************************************************************
 * Window v0.0.0
 *
 * Window is a software plugin, part of the Engine².
 *
 * This file is part of the Engine² project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * Engine² is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file WindowSystem.hpp
 * @brief Window systems declaration.
 *
 * This file contains the declaration of the window systems.
 * The window systems are used to create and manage the windows.
 * All window systems are automatically managed by the Engine² Core.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2025-03-11
 **************************************************************************/

#ifndef WINDOWSYSTEM_HPP_
#define WINDOWSYSTEM_HPP_

#include "Engine.hpp"

#include "Window.hpp"

#include <iostream>

namespace ES::Plugin::Window::System {

const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

void CreateGLFWWindow(ES::Engine::Core &core)
{
    try
    {
        core.RegisterResource<Resource::Window>(
            Resource::Window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "EngineSquarred", nullptr, nullptr));
    }
    catch (const Exception::WindowError &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void GLFWEnableVSync(ES::Engine::Core &core) { glfwSwapInterval(1); }

void PollEvents(ES::Engine::Core &core) { glfwPollEvents(); }

void UpdateKey(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::Window>().GetGLFWWindow();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

} // namespace ES::Plugin::Window::System

#endif /* !WINDOWSYSTEM_HPP_ */
