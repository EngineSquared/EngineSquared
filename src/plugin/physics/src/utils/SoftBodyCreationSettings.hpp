#pragma once

#include <glm/glm.hpp>

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>

namespace ES::Plugin::Physics::Utils {
/// @note Reflects Jolt's SoftBodyCreationSettings.
struct SoftBodyCreationSettings {
    JPH::uint32 numIterations = 5;
    float linearDamping = 0.1f;
    float maxLinearVelocity = 500.0f;
    float restitution = 0.0f;
    float friction = 0.2f;
    float pressure = 0.0f;
    float gravityFactor = 1.0f;
    bool updatePosition = true;
    bool makeRotationIdentity = true;
    bool allowSleeping = true;

    /// @brief Transfers data to a Jolt's SoftBodyCreationSettings.
    /// @param settings Jolt's SoftBodyCreationSettings.
    inline void TransferTo(JPH::SoftBodyCreationSettings &settings) const
    {
        settings.mNumIterations = numIterations;
        settings.mLinearDamping = linearDamping;
        settings.mMaxLinearVelocity = maxLinearVelocity;
        settings.mRestitution = restitution;
        settings.mFriction = friction;
        settings.mPressure = pressure;
        settings.mGravityFactor = gravityFactor;
        settings.mUpdatePosition = updatePosition;
        settings.mMakeRotationIdentity = makeRotationIdentity;
        settings.mAllowSleeping = allowSleeping;
    }
};
} // namespace ES::Plugin::Physics::Utils
