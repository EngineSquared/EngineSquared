#pragma once

#include "utils/AssetID.hpp"

namespace Plugin::Object::Component {
/**
 * Component used to identify a mesh within a game object. MeshID refers to an id
 * of a mesh that should be inside of a ResourceManager.
 */
struct MeshID {
    /**
     * Id refering to a mesh.
     */
    Plugin::Object::Utils::AssetID meshID;
};
} // namespace Plugin::Object::Component
