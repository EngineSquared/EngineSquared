#pragma once

#include <string>

#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
/**
 * CubeMapHandle component
 * This is only a reference to the texture attached to a model of an entity.
 * The handling of the texture is done via the CubeMapManager resource.
 */
struct CubeMapHandle {
    std::string name;

    entt::hashed_string id;

    CubeMapHandle() = default;
    explicit CubeMapHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

    CubeMapHandle(const CubeMapHandle &) = default;
    CubeMapHandle(CubeMapHandle &&) = default;

    CubeMapHandle &operator=(const CubeMapHandle &) = default;
    CubeMapHandle &operator=(CubeMapHandle &&) noexcept = default;
};
} // namespace ES::Plugin::OpenGL::Component
