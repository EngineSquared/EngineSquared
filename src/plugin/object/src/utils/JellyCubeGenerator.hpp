#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

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