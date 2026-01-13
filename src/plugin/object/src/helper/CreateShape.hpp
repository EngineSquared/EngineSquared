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
 * @file CreateShape.hpp
 * @brief Helper functions to generate basic geometric shapes
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

namespace Object::Helper {

/**
 * @brief Create a cube entity with mesh and transform
 *
 * This is a high-level helper that creates an entity with:
 * - Mesh component (cube geometry)
 * - Transform component (position, rotation, scale)
 *
 * @param core Engine core reference
 * @param size Side length of the cube (default: 1.0)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cube entity:"
 * @code
 * auto cube = Object::CreateCube(core, 2.0f, glm::vec3(0, 5, 0));
 * cube.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateCube(Engine::Core &core, float size = 1.0f, const glm::vec3 &position = glm::vec3(0.0f),
                          const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                          const glm::vec3 &scale = glm::vec3(1.0f));

/**
 * @brief Create a sphere entity with mesh and transform
 *
 * @param core Engine core reference
 * @param radius Radius of the sphere (default: 0.5)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @param segments Number of horizontal segments (default: 32)
 * @param rings Number of vertical rings (default: 16)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a sphere entity:"
 * @code
 * auto sphere = Object::CreateSphere(core, 0.5f, glm::vec3(0, 10, 0));
 * sphere.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateSphere(Engine::Core &core, float radius = 0.5f, const glm::vec3 &position = glm::vec3(0.0f),
                            const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                            const glm::vec3 &scale = glm::vec3(1.0f), uint32_t segments = 32u, uint32_t rings = 16u);

/**
 * @brief Create a plane entity with mesh and transform
 *
 * @param core Engine core reference
 * @param width Width of the plane (X axis, default: 1.0)
 * @param depth Depth of the plane (Z axis, default: 1.0)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @param subdivisionsX Number of subdivisions along X (default: 1)
 * @param subdivisionsZ Number of subdivisions along Z (default: 1)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a plane entity:"
 * @code
 * auto floor = Object::CreatePlane(core, 20.0f, 20.0f, glm::vec3(0, 0, 0));
 * floor.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateStatic());
 * @endcode
 */
Engine::Entity CreatePlane(Engine::Core &core, float width = 1.0f, float depth = 1.0f,
                           const glm::vec3 &position = glm::vec3(0.0f),
                           const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                           const glm::vec3 &scale = glm::vec3(1.0f), uint32_t subdivisionsX = 1u,
                           uint32_t subdivisionsZ = 1u);

/**
 * @brief Create a cylinder entity with mesh and transform
 *
 * @param core Engine core reference
 * @param radiusTop Radius at the top (default: 0.5)
 * @param radiusBottom Radius at the bottom (default: 0.5)
 * @param height Height of the cylinder (default: 1.0)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @param segments Number of radial segments (default: 32)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cylinder entity:"
 * @code
 * auto cylinder = Object::CreateCylinder(core, 0.5f, 0.5f, 2.0f, glm::vec3(0, 1, 0));
 * cylinder.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateCylinder(Engine::Core &core, float radiusTop = 0.5f, float radiusBottom = 0.5f,
                              float height = 1.0f, const glm::vec3 &position = glm::vec3(0.0f),
                              const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                              const glm::vec3 &scale = glm::vec3(1.0f), uint32_t segments = 32u);

/**
 * @brief Create a capsule entity with mesh and transform
 *
 * @param core Engine core reference
 * @param radius Radius of the capsule (default: 0.5)
 * @param height Height of the cylindrical part (default: 1.0)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @param segments Number of radial segments (default: 32)
 * @param heightSegments Number of vertical segments for the cylinder (default: 4)
 * @return Engine::Entity The created entity with mesh and transform
 */
Engine::Entity CreateCapsule(Engine::Core &core, float radius = 0.5f, float height = 1.0f,
                             const glm::vec3 &position = glm::vec3(0.0f),
                             const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                             const glm::vec3 &scale = glm::vec3(1.0f), uint32_t segments = 32u,
                             uint32_t heightSegments = 4u);

/**
 * @brief Create a cloth entity with mesh and transform for soft body simulation
 *
 * Creates a 2D grid of vertices in the XY plane, suitable for soft body cloth simulation.
 * After creation, add a SoftBody component to enable physics:
 *
 * @param core Engine core reference
 * @param width Number of vertices along X axis (minimum 2)
 * @param height Number of vertices along Y axis (minimum 2)
 * @param spacing Distance between adjacent vertices (default: 0.1)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cloth for soft body simulation:"
 * @code
 * auto cloth = Object::Helper::CreateCloth(core, 10, 10, 0.1f, glm::vec3(0, 5, 0));
 * auto& soft = cloth.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Cloth(0.5f));
 * soft.PinVertex(0);  // Pin top-left corner
 * soft.PinVertex(9);  // Pin top-right corner
 * @endcode
 */
Engine::Entity CreateCloth(Engine::Core &core, uint32_t width, uint32_t height, float spacing = 0.1f,
                           const glm::vec3 &position = glm::vec3(0.0f),
                           const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                           const glm::vec3 &scale = glm::vec3(1.0f));

/**
 * @brief Create a rope entity with mesh and transform for soft body simulation
 *
 * Creates a 1D chain of vertices along the -Y axis, suitable for soft body rope simulation.
 * After creation, add a SoftBody component to enable physics:
 *
 * @param core Engine core reference
 * @param segmentCount Number of segments (vertices = segmentCount + 1)
 * @param segmentLength Length of each segment (default: 0.1)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a rope for soft body simulation:"
 * @code
 * auto rope = Object::Helper::CreateRope(core, 20, 0.1f, glm::vec3(0, 5, 0));
 * auto& soft = rope.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Rope(0.9f));
 * soft.PinVertex(0);  // Pin top of rope
 * @endcode
 */
Engine::Entity CreateRope(Engine::Core &core, uint32_t segmentCount, float segmentLength = 0.1f,
                          const glm::vec3 &position = glm::vec3(0.0f),
                          const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                          const glm::vec3 &scale = glm::vec3(1.0f));

/**
 * @brief Create a volumetric jelly cube entity for soft body simulation
 *
 * Creates a 3D grid of vertices (gridSize³ total) with surface faces for rendering.
 * Unlike CreateCube (which has only surface vertices), this creates internal vertices
 * needed for volumetric soft body simulation ("jelly" effect).
 *
 * @param core Engine core reference
 * @param gridSize Number of vertices per axis (minimum 2). Total vertices = gridSize³
 * @param spacing Distance between adjacent vertices (default: 0.1)
 * @param position Position in world space (default: origin)
 * @param rotation Rotation as quaternion (default: identity)
 * @param scale Scale factor (default: 1.0)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @note For a standard cube soft body (surface-only deformation), use CreateCube instead.
 *       This function is specifically for volumetric "jelly" simulation.
 *
 * @example "Creating a jelly cube:"
 * @code
 * auto jelly = Object::Helper::CreateJellyCube(core, 5, 0.2f, glm::vec3(0, 5, 0));
 * jelly.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Jelly());
 * @endcode
 */
Engine::Entity CreateJellyCube(Engine::Core &core, uint32_t gridSize, float spacing = 0.1f,
                               const glm::vec3 &position = glm::vec3(0.0f),
                               const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                               const glm::vec3 &scale = glm::vec3(1.0f));

} // namespace Object::Helper
