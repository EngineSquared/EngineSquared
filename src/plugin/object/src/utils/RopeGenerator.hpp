#pragma once

#include "component/Mesh.hpp"
#include <cstdint>
#include <glm/glm.hpp>

namespace Object::Utils {

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

} // namespace Object::Utils
