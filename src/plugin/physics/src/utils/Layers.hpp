#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace ES::Plugin::Physics::Utils {
namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
} // namespace Layers
} // namespace ES::Plugin::Physics::Utils
