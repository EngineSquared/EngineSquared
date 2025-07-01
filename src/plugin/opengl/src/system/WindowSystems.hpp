#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {

void InitGLEW(const ES::Engine::Core &core);
void CheckGLEWVersion(const ES::Engine::Core &core);
void SetupResizeViewport(ES::Engine::Core &core);
void SetupMouseDragging(ES::Engine::Core &core);

void GLClearColor(const ES::Engine::Core &core);
void GLClearDepth(const ES::Engine::Core &core);
void GLEnableDepth(const ES::Engine::Core &core);
void GLEnableCullFace(const ES::Engine::Core &core);

/**
 * @brief Sets the depth function to GL_LEQUAL for skybox rendering.
 *
 * This allows the depth test to pass when values are equal to the depth buffer's content.
 * Useful for rendering skyboxes at maximum depth.
 *
 * @param core Reference to the engine core.
 */
void GLEnableDepthLEqual(const ES::Engine::Core &core);

/**
 * @brief Resets the depth function to its default (GL_LESS).
 *
 * Should be called after skybox rendering to restore normal depth testing.
 *
 * @param core Reference to the engine core.
 */
void GLResetDepthFunc(const ES::Engine::Core &core);

/**
 * @brief Disables depth writing.
 *
 * Useful for rendering skyboxes or transparent objects where depth writes are not needed.
 *
 * @param core Reference to the engine core.
 */
void GLDisableDepthWrite(const ES::Engine::Core &core);

/**
 * @brief Enables depth writing.
 *
 * Should be called after disabling depth writes to restore normal depth buffer behavior.
 *
 * @param core Reference to the engine core.
 */
void GLEnableDepthWrite(const ES::Engine::Core &core);

} // namespace ES::Plugin::OpenGL::System
