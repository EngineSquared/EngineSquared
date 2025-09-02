#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * TextureHandle component
 * This is only a reference to the texture attached to a model of an entity.
 * The handling of the texture is done via the TextureManager resource.
 */
struct TextureHandle {
    std::string name;

    entt::hashed_string id;

    TextureHandle() = default;
    explicit TextureHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    TextureHandle(const TextureHandle &) = default;
    TextureHandle(TextureHandle &&) = default;

    TextureHandle &operator=(const TextureHandle &) = default;
    TextureHandle &operator=(TextureHandle &&) noexcept = default;
};
} // namespace ES::Plugin::OpenGL::Component
