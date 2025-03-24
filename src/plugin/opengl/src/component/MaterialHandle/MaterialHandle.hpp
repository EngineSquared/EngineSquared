#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * MaterialHandle component
 * This is only a reference to the material used by the entity,
 * handling of the material is done via the MaterialCache resource.
 */
struct MaterialHandle {
    std::string name;

    entt::hashed_string id;

    MaterialHandle() = default;
    explicit MaterialHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    MaterialHandle(const MaterialHandle &) = default;
    MaterialHandle(MaterialHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
