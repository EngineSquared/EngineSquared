/**************************************************************************
 * EngineSquared v0.2.0
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
 * @version 0.2.0
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
 * @brief Struct to hold parameters for creating a cube
 */
struct CreateCubeInfo {
    float size = 1.0f;                                      /// Side length of the cube (default: 1.0)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
};

/**
 * @brief Create a cube entity with mesh and transform
 *
 * This is a high-level helper that creates an entity with:
 * - Mesh component (cube geometry)
 * - Transform component (position, rotation, scale)
 *
 * @param core Engine core reference
 * @param info Parameters for creating the cube (size, position, rotation, scale)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cube entity:"
 * @code
 * auto cube = Object::CreateCube(core, { .size = 2.0f, .position = glm::vec3(0, 5, 0) });
 * cube.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateCube(Engine::Core &core, CreateCubeInfo info = {});

/**
 * @brief Struct to hold parameters for creating a sphere
 */
struct CreateSphereInfo {
    float radius = 0.5f;                                    /// Radius of the sphere (default: 0.5)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
    uint32_t segments = 32u;                                /// Number of horizontal segments (default: 32)
    uint32_t rings = 16u;                                   /// Number of vertical rings (default: 16)
};

/**
 * @brief Create a sphere entity with mesh and transform
 *
 * @param core Engine core reference
 * @param info Parameters for creating the sphere (radius, position, rotation, scale, segments, rings)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a sphere entity:"
 * @code
 * auto sphere = Object::CreateSphere(core, { .radius = 0.5f, .position = glm::vec3(0, 10, 0) });
 * sphere.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateSphere(Engine::Core &core, CreateSphereInfo info = {});

/**
 * @brief Struct to hold parameters for creating a plane
 */
struct CreatePlaneInfo {
    float width = 1.0f;                                     /// Width of the plane (X axis, default: 1.0)
    float depth = 1.0f;                                     /// Depth of the plane (Z axis, default: 1.0)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
    uint32_t subdivisionsX = 1u;                            /// Number of subdivisions along X (default: 1)
    uint32_t subdivisionsZ = 1u;                            /// Number of subdivisions along Z (default: 1)
};

/**
 * @brief Create a plane entity with mesh and transform
 *
 * @param core Engine core reference
 * @param info Parameters for creating the plane (width, depth, position, rotation, scale, subdivisionsX, subdivisionsZ)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a plane entity:"
 * @code
 * auto floor = Object::CreatePlane(core, { .width = 20.0f, .depth = 20.0f, .position = glm::vec3(0, 0, 0) });
 * floor.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateStatic());
 * @endcode
 */
Engine::Entity CreatePlane(Engine::Core &core, CreatePlaneInfo info = {});

/**
 * @brief Struct to hold parameters for creating a cylinder
 */
struct CreateCylinderInfo {
    float radiusTop = 0.5f;                                 /// Radius at the top (default: 0.5)
    float radiusBottom = 0.5f;                              /// Radius at the bottom (default: 0.5)
    float height = 1.0f;                                    /// Height of the cylinder (default: 1.0)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
    uint32_t segments = 32u;                                /// Number of radial segments (default: 32)
};

/**
 * @brief Create a cylinder entity with mesh and transform
 *
 * @param core Engine core reference
 * @param info Parameters for creating the cylinder (radiusTop, radiusBottom, height, position, rotation, scale,
 * segments)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cylinder entity:"
 * @code
 * auto cylinder = Object::CreateCylinder(core, { .radiusTop = 0.5f, .radiusBottom = 0.5f, .height = 2.0f, .position =
 * glm::vec3(0, 1, 0) }); cylinder.AddComponent<Physics::RigidBody>(core, Physics::RigidBody::CreateDynamic());
 * @endcode
 */
Engine::Entity CreateCylinder(Engine::Core &core, CreateCylinderInfo info = {});

/**
 * @brief Struct to hold parameters for creating a capsule
 */
struct CreateCapsuleInfo {
    float radius = 0.5f;                                    /// Radius of the capsule (default: 0.5)
    float height = 1.0f;                                    /// Height of the cylindrical part (default: 1.0)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
    uint32_t segments = 32u;                                /// Number of radial segments (default: 32)
    uint32_t heightSegments = 4u;                           /// Number of vertical segments for the cylinder (default: 4)
};

/**
 * @brief Create a capsule entity with mesh and transform
 *
 * @param core Engine core reference
 * @param info Parameters for creating the capsule (radius, height, position, rotation, scale, segments, heightSegments)
 * @return Engine::Entity The created entity with mesh and transform
 */
Engine::Entity CreateCapsule(Engine::Core &core, CreateCapsuleInfo info = {});

/**
 * @brief Struct to hold parameters for creating a cloth
 */
struct CreateClothInfo {
    uint32_t width = 10u;                                   /// Number of vertices along X axis (default: 10)
    uint32_t height = 10u;                                  /// Number of vertices along Y axis (default: 10)
    float spacing = 0.1f;                                   /// Distance between adjacent vertices (default: 0.1)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
};

/**
 * @brief Create a cloth entity with mesh and transform for soft body simulation
 *
 * Creates a 2D grid of vertices in the XY plane, suitable for soft body cloth simulation.
 * After creation, add a SoftBody component to enable physics:
 *
 * @param core Engine core reference
 * @param info Parameters for creating the cloth (width, height, spacing, position, rotation, scale)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a cloth for soft body simulation:"
 * @code
 * auto cloth = Object::Helper::CreateCloth(core, { .width = 10, .height = 10, .spacing = 0.1f, .position = glm::vec3(0,
 * 5, 0) }); auto& soft = cloth.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Cloth(0.5f));
 * soft.PinVertex(0);  // Pin top-left corner
 * soft.PinVertex(9);  // Pin top-right corner
 * @endcode
 */
Engine::Entity CreateCloth(Engine::Core &core, CreateClothInfo info = {});

/**
 * @brief Struct to hold parameters for creating a rope
 */
struct CreateRopeInfo {
    uint32_t segmentCount = 10u;                            /// Number of segments (default: 10)
    float segmentLength = 0.1f;                             /// Length of each segment (default: 0.1)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
};

/**
 * @brief Create a rope entity with mesh and transform for soft body simulation
 *
 * Creates a 1D chain of vertices along the -Y axis, suitable for soft body rope simulation.
 * After creation, add a SoftBody component to enable physics:
 *
 * @param core Engine core reference
 * @param info Parameters for creating the rope (segmentCount, segmentLength, position, rotation, scale)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @example "Creating a rope for soft body simulation:"
 * @code
 * auto rope = Object::Helper::CreateRope(core, { .segmentCount = 20, .segmentLength = 0.1f, .position = glm::vec3(0, 5,
 * 0) }); auto& soft = rope.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Rope(0.9f));
 * soft.PinVertex(0);  // Pin top of rope
 * @endcode
 */
Engine::Entity CreateRope(Engine::Core &core, CreateRopeInfo info = {});

/**
 * @brief Struct to hold parameters for creating a jelly cube
 */
struct CreateJellyCubeInfo {
    float size = 1.0f;                                      /// Side length of the cube (default: 1.0)
    uint32_t gridSize = 5u;                                 /// Number of vertices per axis (default: 5)
    glm::vec3 position = glm::vec3(0.0f);                   /// Position in world space (default: origin)
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); /// Rotation as quaternion (default: identity)
    glm::vec3 scale = glm::vec3(1.0f);                      /// Scale factor (default: 1.0)
};

/**
 * @brief Create a volumetric jelly cube entity for soft body simulation
 *
 * Creates a 3D grid of vertices (gridSize³ total) with surface faces for rendering.
 * Unlike CreateCube (which has only surface vertices), this creates internal vertices
 * needed for volumetric soft body simulation ("jelly" effect).
 *
 * The spacing between vertices is automatically calculated as: size / (gridSize - 1)
 *
 * @param core Engine core reference
 * @param info Parameters for creating the jelly cube (size, gridSize, position, rotation, scale)
 * @return Engine::Entity The created entity with mesh and transform
 *
 * @note For a standard cube soft body (surface-only deformation), use CreateCube instead.
 *       This function is specifically for volumetric "jelly" simulation.
 *
 * @example "Creating a 2-unit jelly cube with 5x5x5 grid:"
 * @code
 * auto jelly = Object::Helper::CreateJellyCube(core, { .size = 2.0f, .gridSize = 5, .position = glm::vec3(0, 5, 0) });
 * jelly.AddComponent<Physics::Component::SoftBody>(core, SoftBodySettings::Jelly());
 * @endcode
 */
Engine::Entity CreateJellyCube(Engine::Core &core, CreateJellyCubeInfo info = {});

} // namespace Object::Helper
