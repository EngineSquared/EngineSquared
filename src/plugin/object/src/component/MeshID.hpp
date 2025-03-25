#pragma once

#include <entt/entt.hpp>

namespace ES::Plugin::Object::Component {
/**
 * Component used to identify a mesh within a game object. MeshID refers to an id
 * of a mesh that should be inside of a ResourceManager.
 */
struct MeshID {
    /**
     * Id refering to a mesh.
     */
    entt::hashed_string meshID;
};
} // namespace ES::Plugin::Object::Component
