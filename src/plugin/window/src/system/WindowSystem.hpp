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

/**
 * @brief Create a GLFW window.
 *
 * This function creates a GLFW window.
 *
 * @param core  The Engine² Core.
 */
void CreateWindow(ES::Engine::Core &core);

/**
 * @brief Enable VSync.
 *
 * This function enables VSync.
 *
 * @param core  The Engine² Core.
 */
void EnableVSync(ES::Engine::Core &core);

/**
 * @brief Poll GLFW events.
 *
 * This function polls GLFW events.
 *
 * @param core  The Engine² Core.
 */
void PollEvents(ES::Engine::Core &core);

/**
 * @brief Update the key.
 *
 * This function updates the key.
 *
 * @param core  The Engine² Core.
 */
void UpdateKey(ES::Engine::Core &core);

} // namespace ES::Plugin::Window::System

#endif /* !WINDOWSYSTEM_HPP_ */
