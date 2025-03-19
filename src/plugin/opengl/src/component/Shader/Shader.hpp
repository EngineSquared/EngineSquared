#pragma once

#include <string>

namespace ES::Plugin::OpenGL::Component {
/**
 * Shader component
 * This is only a reference to the shader program used by the entity,
 * handling of the shader program is done via the ShaderManager resource.
 */
struct Shader {
    std::string name;
};
} // namespace ES::Plugin::OpenGL::Component
