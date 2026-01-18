#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

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

} // namespace Object::Utils