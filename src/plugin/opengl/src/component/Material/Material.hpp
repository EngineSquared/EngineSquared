#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * Material component
 * This is only a reference to the material used by the entity,
 * handling of the material is done via the MaterialCache resource.
 */
struct Material {
    std::string name;

    entt::hashed_string id;

    Material() = default;
    Material(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    Material(const Material &) = default;
    Material(Material &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
