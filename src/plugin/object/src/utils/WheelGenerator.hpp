#pragma once

#include "component/Mesh.hpp"
#include <cstdint>
#include <glm/glm.hpp>

namespace Object::Utils {

/**
 * @brief Generate a wheel mesh (cylinder oriented along the X axis)
 *
 * Creates a cylinder mesh rotated 90 degrees so the wheel's axis of rotation
 * is along the X axis (left-right), suitable for vehicle wheels.
 *
 * @param radius Radius of the wheel (default: 0.3)
 * @param width Width of the wheel (default: 0.2)
 * @param segments Number of radial segments (default: 24)
 * @return Component::Mesh The generated wheel mesh
 */
Component::Mesh GenerateWheelMesh(float radius = 0.3f, float width = 0.2f, uint32_t segments = 24u);

} // namespace Object::Utils
