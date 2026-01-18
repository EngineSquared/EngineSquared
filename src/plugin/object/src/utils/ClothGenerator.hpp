#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

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

} // namespace Object::Utils