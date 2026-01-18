#pragma once

#include "component/Mesh.hpp"
#include <cstdint>
#include <glm/glm.hpp>

namespace Object::Utils {

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
