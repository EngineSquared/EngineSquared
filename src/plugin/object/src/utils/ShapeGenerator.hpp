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

/**
 * @brief Generate a cloth mesh (2D grid in XY plane)
 *
 * Creates a grid of vertices suitable for soft body cloth simulation.
 * The mesh is generated in the XY plane, facing +Z direction.
 *
 * @param width Number of vertices along X axis (minimum 2)
 * @param height Number of vertices along Y axis (minimum 2)
 * @param spacing Distance between adjacent vertices (default: 0.1)
 * @return Component::Mesh The generated cloth mesh with vertices, normals, UVs, and indices
 *
 * @note For degenerate sizes (width < 2 or height < 2), returns vertices without faces.
 *
 * @example "Creating a cloth for soft body simulation:"
 * @code
 * auto mesh = Object::Utils::GenerateClothMesh(10, 10, 0.1f);
 * entity.AddComponent<Mesh>(core, mesh);
 * entity.AddComponent<SoftBody>(core, SoftBodySettings::Cloth(0.5f));
 * @endcode
 */
Component::Mesh GenerateClothMesh(uint32_t width, uint32_t height, float spacing = 0.1f);

/**
 * @brief Generate a rope mesh (1D chain of vertices along -Y axis)
 *
 * Creates a line of vertices suitable for soft body rope simulation.
 * The rope extends downward from origin along the -Y axis.
 *
 * @param segmentCount Number of segments (vertices = segmentCount + 1)
 * @param segmentLength Length of each segment (default: 0.1)
 * @return Component::Mesh The generated rope mesh with vertices and normals (no faces)
 *
 * @note Rope meshes have no faces/indices - they are meant for line rendering
 *       or soft body simulation only.
 *
 * @example "Creating a rope for soft body simulation:"
 * @code
 * auto mesh = Object::Utils::GenerateRopeMesh(20, 0.1f);
 * entity.AddComponent<Mesh>(core, mesh);
 * entity.AddComponent<SoftBody>(core, SoftBodySettings::Rope(0.9f));
 * @endcode
 */
Component::Mesh GenerateRopeMesh(uint32_t segmentCount, float segmentLength = 0.1f);

/**
 * @brief Generate a volumetric jelly cube mesh (3D grid)
 *
 * Creates a 3D grid of vertices (gridSize³ total) with surface faces for rendering.
 * Unlike GenerateCubeMesh (surface-only), this creates internal vertices needed
 * for volumetric soft body simulation.
 *
 * @param gridSize Number of vertices per axis (minimum 2). Total vertices = gridSize³
 * @param spacing Distance between adjacent vertices (default: 0.1)
 * @return Component::Mesh The generated cube mesh with surface faces
 *
 * @note Only surface faces are generated for rendering. Internal vertices are
 *       used by the soft body physics system for volumetric simulation.
 *
 * @example "Creating a jelly cube:"
 * @code
 * auto mesh = Object::Utils::GenerateJellyCubeMesh(5, 0.2f);
 * entity.AddComponent<Mesh>(core, mesh);
 * entity.AddComponent<SoftBody>(core, SoftBodySettings::Jelly());
 * @endcode
 */
Component::Mesh GenerateJellyCubeMesh(uint32_t gridSize, float spacing = 0.1f);

} // namespace Object::Utils
