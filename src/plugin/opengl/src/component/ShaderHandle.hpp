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
    bool operator==(const ShaderHandle &other) const { return name == other.name && id == other.id; }
};

} // namespace ES::Plugin::OpenGL::Component

namespace std {
// Custom hash specialization for ShaderHandle
template <> struct hash<ES::Plugin::OpenGL::Component::ShaderHandle> {
    std::size_t operator()(const ES::Plugin::OpenGL::Component::ShaderHandle &shaderHandle) const noexcept
    {
        return std::hash<std::string>()(shaderHandle.name) ^
               (std::hash<std::size_t>()(static_cast<std::size_t>(shaderHandle.id.value())) << 1);
    }
};
} // namespace std
