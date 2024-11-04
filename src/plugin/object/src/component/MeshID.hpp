#pragma once

#include "AssetID.hpp"

namespace ES::Plugin::Object::Component {
/**
 * Component used to identify a mesh within a game object. MeshID refers to an id of a mesh that should be inside of a
 * AssetsManager.
 */
struct MeshID {
    /**
     * Id refering to a mesh.
     */
    ES::Plugin::Object::Utils::AssetID meshID;
};
} // namespace ES::Plugin::Object::Component
