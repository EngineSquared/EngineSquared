#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * FontHandle component
 * This is only a reference to the font used by the entity,
 * handling of the font is done via the FontCache resource.
 */
struct FontHandle {
    std::string name;

    entt::hashed_string id;

    FontHandle() = default;
    explicit FontHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    FontHandle(const FontHandle &) = default;
    FontHandle(FontHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
