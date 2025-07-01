/**************************************************************************
 * EngineSquared v0.0.0
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file CreateSkyBox.hpp
 * @brief Utility functions to create skybox entities in the EngineSquared framework
 *
 * This header provides functions to create skybox entities using either a single cross-layout texture
 * or six separate textures for each face of the skybox. The created entities include necessary components
 * such as Transform, Mesh, CubeMapHandle, ModelHandle, MaterialHandle, and ShaderHandle.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2025-06-15
 **************************************************************************/

#pragma once

#include "Engine.hpp"
#include "Object.hpp"

#include <array>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <string_view>

namespace ES::Plugin::OpenGL::Utils {

/**
 * @brief Creates a skybox entity from a cross-layout texture
 *
 * Generates a skybox entity with mesh, material, and shader components from a cross-format
 * image. The function creates a cube mesh and loads the texture through the CubeMapManager.
 *
 * @param core Reference to the engine core
 * @param texture_path Path to the cross-layout texture image
 * @param world_position 3D position of the skybox in world space (default: origin)
 * @param world_rotation Quaternion rotation of the skybox (default: identity)
 * @param skybox_dimensions Skybox cube dimensions (default: unit cube)
 * @return ES::Engine::Entity Skybox entity with Transform, Mesh, CubeMapHandle, ModelHandle, MaterialHandle, and
 * ShaderHandle components
 *
 * @throws OpenGLError If texture_path is empty or skybox_dimensions contains non-positive values
 * @throws OpenGLError If texture loading fails
 * @note Creates a cube mesh with 36 vertices and 36 indices (no face optimization)
 * @note Adds "skyboxDefault" material and shader handles
 * @note Resource ID format: "cubemap_cross_" + filename (without extension)
 * @note CRITICAL: Requires proper rendering order - skybox must be rendered LAST with GL_LEQUAL depth function
 *
 * @code
 * * Basic skybox creation
 * auto skybox = ES::Plugin::OpenGL::Utils::CreateSkyBox(core, "textures/skybox_cross.png");
 *
 * * Large skybox for outdoor scenes
 * auto skybox = ES::Plugin::OpenGL::Utils::CreateSkyBox(core, "textures/sky.png",
 *                           glm::vec3(0.0f, 0.0f, 0.0f),
 *                           glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
 *                           glm::vec3(100.0f, 100.0f, 100.0f));
 * @endcode
 */
ES::Engine::Entity CreateSkyBox(ES::Engine::Core &core, std::string_view texture_path,
                                const glm::vec3 &world_position = glm::vec3{0.0f},
                                const glm::quat &world_rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                                const glm::vec3 &skybox_dimensions = glm::vec3{1.0f});

/**
 * @brief Creates a skybox entity from 6 separate texture files
 *
 * Generates a skybox from 6 distinct images for each cube face through the CubeMapManager.
 * Creates the same components as the cross-layout version.
 *
 * @param core Reference to the engine core
 * @param texture_paths Array of exactly 6 texture file paths for cube faces
 *                     Order: [Right(+X), Left(-X), Top(+Y), Bottom(-Y), Front(+Z), Back(-Z)]
 * @param world_position 3D position of the skybox in world space (default: origin)
 * @param world_rotation Quaternion rotation of the skybox (default: identity)
 * @param skybox_dimensions Skybox cube dimensions (default: unit cube)
 * @return ES::Engine::Entity Skybox entity with Transform, Mesh, CubeMapHandle, ModelHandle, MaterialHandle, and
 * ShaderHandle components
 *
 * @throws OpenGLError If any texture_path is empty or skybox_dimensions contains non-positive values
 * @throws OpenGLError If texture loading fails
 * @note Creates a cube mesh with 36 vertices and 36 indices (no face optimization)
 * @note Adds "skyboxDefault" material and shader handles
 * @note Resource ID format: "cubemap_faces_" + first filename (without extension)
 * @note CRITICAL: Requires proper rendering order - skybox must be rendered LAST with GL_LEQUAL depth function
 *
 * @code
 * * Standard cubemap with 6 separate textures
 * std::array<std::string, 6> skybox_faces = {
 *     "textures/right.jpg",   * Right (+X)
 *     "textures/left.jpg",    * Left (-X)
 *     "textures/top.jpg",     * Top (+Y)
 *     "textures/bottom.jpg",  * Bottom (-Y)
 *     "textures/front.jpg",   * Front (+Z)
 *     "textures/back.jpg"     * Back (-Z)
 * };
 * auto skybox = ES::Plugin::OpenGL::Utils::CreateSkyBox(core, skybox_faces);
 *
 * * Large outdoor environment skybox
 * std::array<std::string, 6> mountain_skybox = {
 *     "textures/mountain_rt.png", "textures/mountain_lf.png",
 *     "textures/mountain_up.png", "textures/mountain_dn.png",
 *     "textures/mountain_ft.png", "textures/mountain_bk.png"
 * };
 * auto outdoor_skybox = ES::Plugin::OpenGL::Utils::CreateSkyBox(core, mountain_skybox,
 *                                   glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
 *                                   glm::vec3(500.0f));
 * @endcode
 */
ES::Engine::Entity CreateSkyBox(ES::Engine::Core &core, const std::array<std::string, 6> &texture_paths,
                                const glm::vec3 &world_position = glm::vec3{0.0f},
                                const glm::quat &world_rotation = glm::quat{1.0f, 0.0f, 0.0f, 0.0f},
                                const glm::vec3 &skybox_dimensions = glm::vec3{1.0f});

} // namespace ES::Plugin::OpenGL::Utils
