#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * ShaderHandle component
 * This is only a reference to the shader program used by the entity,
 * handling of the shader program is done via the ShaderManager resource.
 */
struct ShaderHandle {
    std::string name;

    entt::hashed_string id;

    ShaderHandle() = default;
    explicit ShaderHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    ShaderHandle(const ShaderHandle &) = default;
    ShaderHandle(ShaderHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
