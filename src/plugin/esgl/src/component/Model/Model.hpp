#pragma once

// #include <Windows.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <array>

#include "Loader.hpp"
#include "Material.hpp"
#include "utils/Mesh/Mesh.hpp"

namespace ESGL {
    struct Model {
        std::string shaderName;
        std::string materialName;
        Mesh mesh;
    };
}