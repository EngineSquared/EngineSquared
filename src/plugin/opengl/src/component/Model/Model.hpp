#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>

#include <array>
#include <vector>

#include "GLBuffer.hpp"
#include "Loader.hpp"
#include "Material.hpp"

namespace ES::Plugin::OpenGL::Component {
struct Model {
    std::string shaderName;
    std::string materialName;
    std::string glbufferName;
};
} // namespace ES::Plugin::OpenGL::Component
