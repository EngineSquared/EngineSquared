#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

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

} // namespace Object::Utils