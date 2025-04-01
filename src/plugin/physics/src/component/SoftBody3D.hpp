#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <memory>
#include <glm/glm.hpp>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

#include "Layers.hpp"
#include "SoftBodyVertexSettings.hpp"
#include "SoftBodyCreationSettings.hpp"

namespace ES::Plugin::Physics::Component {
    /// @brief A component that represents any 3D soft body object in the physics world.
    /// @note Before adding a soft body component to an entity, it should have a transform and a mesh component.
    struct SoftBody3D {
        /// @brief A reference to the soft body itself used by Jolt.
        /// @note This should not be constructed manually, this is handled by the systems.
        JPH::Body *body;

        /// @brief A reference to the shared settings of the soft body.
        /// @note This should not be constructed manually, this is handled by the systems.
        std::shared_ptr<JPH::SoftBodySharedSettings> settings;

        /// @brief A reference to the vertex attributes of the soft body.
        JPH::SoftBodySharedSettings::VertexAttributes vertexAttributes;

        /// @brief Layer of the soft body.
        JPH::ObjectLayer layer;

        /// @brief Soft body vertex settings.
        Utils::SoftBodyVertexSettings vertexSettings;

        /// @brief Soft body creation settings.
        Utils::SoftBodyCreationSettings creationSettings;

        /// @brief Whether to calculate volume constraint volumes.
        bool calculateVolumeConstraintVolumes = true;

        /// @brief Whether to calculate skinned constraint normals.
        bool calculateSkinnedConstraintNormals = true;

        /// @brief Construct a soft body with a shape.
        SoftBody3D(const JPH::ObjectLayer _layer = Utils::Layers::MOVING)
            : body(nullptr), settings(nullptr), vertexAttributes(), layer(_layer), vertexSettings(), creationSettings() {}
    };
}