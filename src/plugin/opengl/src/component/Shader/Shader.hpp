#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * Shader component
 * This is only a reference to the shader program used by the entity,
 * handling of the shader program is done via the ShaderManager resource.
 */
struct Shader {
    std::string name;

    entt::hashed_string id;

    Shader() = default;
    Shader(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    Shader(const Shader &) = default;
    Shader(Shader &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
