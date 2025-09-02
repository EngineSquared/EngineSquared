#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
struct TextHandle {
    std::string name;

    entt::hashed_string id;

    TextHandle() = default;
    explicit TextHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    TextHandle(const TextHandle &) = default;
    TextHandle(TextHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
