#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * SpriteHandle component
 * This is only a reference to the Sprite program used by the entity,
 * handling of the Sprite program is done via the SpriteManager resource.
 */
struct SpriteHandle {
    std::string name;

    entt::hashed_string id;

    SpriteHandle() = default;
    explicit SpriteHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    SpriteHandle(const SpriteHandle &) = default;
    SpriteHandle(SpriteHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
