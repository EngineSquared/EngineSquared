#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
struct Model {
    std::string name;

    entt::hashed_string id;

    Model() = default;
    explicit Model(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    Model(const Model &) = default;
    Model(Model &&) = default;
};
} // namespace ES::Plugin::OpenGL::Component
