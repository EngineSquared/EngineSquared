#pragma once

#include "Layers.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Physics::Utils {
namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::BroadPhaseLayer DEBRIS(2);  // For attached softbodies
static constexpr JPH::uint NUM_LAYERS(3);
}; // namespace BroadPhaseLayers
} // namespace Physics::Utils
