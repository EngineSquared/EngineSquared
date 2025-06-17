#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {

void InitGLEW(const ES::Engine::Core &core);
void CheckGLEWVersion(const ES::Engine::Core &core);
void SetupResizeViewport(ES::Engine::Core &core);
void SetupMouseDragging(ES::Engine::Core &core);

/**
 * @brief Initializes the GLEW library for OpenGL function loading.
 *
 * This function initializes GLEW, which is necessary for using modern OpenGL features.
 * It logs an error if initialization fails.
 *
 * @param core Reference to the engine core.
 */
void GLClearColor(const ES::Engine::Core &core);

/**
 * @brief Clears the OpenGL depth buffer.
 *
 * This function clears the depth buffer, which is used to keep track of the depth of pixels
 * in the rendered scene, ensuring that objects are drawn in the correct order based on their distance from the camera.
 *
 * @param core Reference to the engine core.
 */
void GLClearDepth(const ES::Engine::Core &core);

/**
 * @brief Enables OpenGL's depth testing.
 *
 * This function enables the depth test, which is used to determine
 * whether a pixel should be drawn based on its depth value.
 *
 * @param core Reference to the engine core.
 */
void GLEnableDepth(const ES::Engine::Core &core);

/**
 * @brief Enables backface culling to improve rendering performance.
 *
 * This function enables OpenGL's backface culling, which skips rendering
 * polygons facing away from the camera, thus optimizing the rendering process.
 *
 * @param core Reference to the engine core.
 */
void GLEnableCullFace(const ES::Engine::Core &core);

/**
 * @brief Enables multisampling (MSAA) for smoother rendering.
 *
 * This function enables OpenGL's multisample anti-aliasing feature,
 * which helps reduce visual artifacts in rendered images.
 *
 * @param core Reference to the engine core.
 */
void GlEnableMultiSample(const ES::Engine::Core &core);

} // namespace ES::Plugin::OpenGL::System
