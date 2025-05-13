#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <glm/glm.hpp>
#include <memory>

#include "Layers.hpp"

namespace ES::Plugin::Physics::Component {
/// @brief A component that represents any 3D rigid body object in the physics world.
struct RigidBody3D {
    /// @brief A reference to the shape of the rigid body.
    /// It is a shared pointer of a Jolt's ShapeSettings, so any class that inherits from ShapeSettings can be used.
    std::shared_ptr<JPH::ShapeSettings> shapeSettings;

    /// @brief A reference to the rigid body itself used by Jolt.
    /// @note This should not be constructed manually, this is handled by the systems.
    /// @note Memory management is handled by the physics system.
    JPH::Body *body;

    /// @brief Motion type of the rigid body.
    JPH::EMotionType motionType;

    /// @brief Layer of the rigid body.
    JPH::ObjectLayer layer;

    /// @brief Whether the rigid body should be constructed as a sensor or not.
    /// @note A sensor will receive collision callbacks, but will not cause any collision responses
    /// and can be used as a trigger volume.
    bool isSensor = false;

    /// @brief Construct a rigid body with a shape.
    /// @param shapeSettings
    RigidBody3D(const std::shared_ptr<JPH::ShapeSettings> &_shapeSettings,
                const JPH::EMotionType _motionType = JPH::EMotionType::Static,
                const JPH::ObjectLayer _layer = Utils::Layers::NON_MOVING, const bool _isSensor = false)
        : shapeSettings(_shapeSettings), body(nullptr), motionType(_motionType), layer(_layer), isSensor(_isSensor)
    {
        // Make sure Jolt doesn't try to free the settings, the shared_ptr will do it.
        shapeSettings->SetEmbedded();
    }
};
} // namespace ES::Plugin::Physics::Component
