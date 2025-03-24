#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
struct ModelHandle {
    std::string name;

    entt::hashed_string id;

    ModelHandle() = default;
    explicit ModelHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    ModelHandle(const ModelHandle &) = default;
    ModelHandle(ModelHandle &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
