#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Sync wheel entity transforms with Jolt vehicle wheel positions
 *
 * Should run every physics tick (FixedTimeUpdate scheduler) AFTER PhysicsUpdate.
 * Updates the Transform component of each wheel entity to match the computed
 * wheel position and rotation from the vehicle constraint.
 */
void WheelTransformSyncSystem(Engine::Core &core);

} // namespace Physics::System
