/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file ShapeGenerator.hpp
 * @brief Utility functions to generate basic geometric shapes
 *
 * This file provides helper functions to create common 3D shapes like cubes,
 * spheres, planes, etc. with proper vertices, normals, texture coordinates,
 * and indices.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"

#include <glm/glm.hpp>

namespace Object::Utils {

/**
 * @brief Generate a cube mesh with specified size
 *
 * @param size Side length of the cube (default: 1.0)
 * @return Component::Mesh The generated cube mesh with vertices, normals, UVs, and indices
 */
Component::Mesh GenerateCubeMesh(float size = 1.0f);

/**
 * @brief Generate a sphere mesh with specified radius and subdivisions
 *
 * Uses UV sphere generation (latitude/longitude grid)
 *
 * @param radius Radius of the sphere (default: 0.5)
 * @param segments Number of horizontal segments (default: 32)
 * @param rings Number of vertical rings (default: 16)
 * @return Component::Mesh The generated sphere mesh
 */
Component::Mesh GenerateSphereMesh(float radius = 0.5f, uint32_t segments = 32u, uint32_t rings = 16u);

/**
 * @brief Generate a plane mesh with specified width and depth
 *
 * @param width Width of the plane (X axis, default: 1.0)
 * @param depth Depth of the plane (Z axis, default: 1.0)
 * @param subdivisionsX Number of subdivisions along X (default: 1)
 * @param subdivisionsZ Number of subdivisions along Z (default: 1)
 * @return Component::Mesh The generated plane mesh
 */
Component::Mesh GeneratePlaneMesh(float width = 1.0f, float depth = 1.0f, uint32_t subdivisionsX = 1u,
                                  uint32_t subdivisionsZ = 1u);

/**
 * @brief Generate a cylinder mesh with specified dimensions
 *
 * @param radiusTop Radius at the top (default: 0.5)
 * @param radiusBottom Radius at the bottom (default: 0.5)
 * @param height Height of the cylinder (default: 1.0)
 * @param segments Number of radial segments (default: 32)
 * @param heightSegments Number of height segments (default: 1)
 * @return Component::Mesh The generated cylinder mesh
 */
Component::Mesh GenerateCylinderMesh(float radiusTop = 0.5f, float radiusBottom = 0.5f, float height = 1.0f,
                                     uint32_t segments = 32u, uint32_t heightSegments = 1u);

/**
 * @brief Generate a capsule mesh with given radius and cylinder height
 *
 * @param radius Radius of the capsule
 * @param height Height of the cylindrical middle section
 * @param segments Radial segments
 * @param heightSegments Number of subdivisions along the cylinder height
 * @return Component::Mesh The generated capsule mesh
 */
Component::Mesh GenerateCapsuleMesh(float radius = 0.5f, float height = 1.0f, uint32_t segments = 32u,
                                    uint32_t heightSegments = 4u);

} // namespace Object::Utils
