#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include "entity/EntityId.hpp"
#include <Jolt/Physics/Body/BodyID.h>

#include "utils/BiMap.hpp"

namespace Physics::Resource {
using BodyEntityMap = Utils::BiMap<Engine::EntityId, JPH::BodyID>;
} // namespace Physics::Resource
