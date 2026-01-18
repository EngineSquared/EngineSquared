#pragma once

#include "component/Mesh.hpp"
#include <cstdint>
#include <glm/glm.hpp>

namespace Object::Utils {

/**
 * @brief Generate a box mesh with specified dimensions
 *
 * @param width Width of the box
 * @param height Height of the box
 * @param depth Depth of the box
 * @return Component::Mesh The generated box mesh with vertices, normals, UVs, and indices
 */
Component::Mesh GenerateBoxMesh(float width, float height, float depth);

} // namespace Object::Utils