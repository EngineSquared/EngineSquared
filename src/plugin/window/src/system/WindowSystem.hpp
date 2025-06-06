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

#pragma once

#include "Engine.hpp"
#include "Window/Window.hpp"

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
void CreateWindowSystem(ES::Engine::Core &core);

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
 * @brief Initialize the GLFW
 *
 * This function initialize GLFW.
 *
 * @param core  The Engine² Core.
 */
void InitGLFW(const ES::Engine::Core &core);

/**
 * @brief Setup GLFW Hints
 *
 * This function setup the GLFW Hints.
 *
 * @param core  The Engine² Core.
 */
void SetupGLFWHints(const ES::Engine::Core &core);

/**
 * @brief Link the GLFW context to GL
 *
 * This function link the GLFW context to GL.
 *
 * @param core  The Engine² Core.
 */
void LinkGLFWContextToGL(ES::Engine::Core &core);

/**
 * @brief Swap the buffers
 *
 * This function swaps the buffers.
 *
 * @param core  The Engine² Core.
 */
void SwapBuffers(ES::Engine::Core &core);

/**
 * @brief Stop the window systems
 *
 * This function stops the window systems.
 *
 * @param core  The Engine² Core.
 */
void StopSystems(ES::Engine::Core &core);

/**
 * @brief Store a pointer to the currently used Core in the GLFW window
 *
 * @param core  The Engine² Core.
 *
 * @note This can be used later to access the Core from the GLFW window,
 * for example in callbacks where the Core is not directly accessible.
 */
void StoreCoreInWindow(ES::Engine::Core &core);

/**
 * @brief Destroy the GLFW window
 *
 * This function destroys the GLFW window.
 *
 * @param core  The Engine² Core.
 */
void DestroyWindow(ES::Engine::Core &core);

} // namespace ES::Plugin::Window::System
