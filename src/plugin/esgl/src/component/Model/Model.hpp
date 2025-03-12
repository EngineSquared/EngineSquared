#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>
#include <vector>

#include "Loader.hpp"
#include "Material.hpp"
#include "utils/Mesh/Mesh.hpp"

namespace ES::Plugin::ESGL {
struct Model {
    std::string shaderName;
    std::string materialName;
    Mesh mesh;
};
} // namespace ES::Plugin::ESGL
