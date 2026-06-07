#pragma once
#include "entity/EntityId.hpp"

namespace Physics {
struct HitRecord {
    float t;
    Engine::EntityId hittedEntityId;
};
} // namespace Physics
