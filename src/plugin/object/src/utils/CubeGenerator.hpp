#pragma once

#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

namespace Object::Utils {

/**
 * @brief Generate a cube mesh with specified size
 *
 * @param size Side length of the cube (default: 1.0)
 * @return Component::Mesh The generated cube mesh with vertices, normals, UVs, and indices
 */
Component::Mesh GenerateCubeMesh(float size = 1.0f);

} // namespace Object::Utils