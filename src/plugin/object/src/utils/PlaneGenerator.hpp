#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

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

} // namespace Object::Utils