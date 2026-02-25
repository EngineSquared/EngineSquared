#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Physics::Utils {
namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer DEBRIS = 2;  // For attached softbodies, debris - only collides with NON_MOVING
static constexpr JPH::ObjectLayer NUM_LAYERS = 3;
} // namespace Layers
} // namespace Physics::Utils
