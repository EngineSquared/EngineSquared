#pragma once
#include "entity/EntityId.hpp"

namespace Physics {
    struct HitRecord {
        float t; /// Represent the length of the distance 
        Engine::EntityId hittedEntityId;
    };
}
