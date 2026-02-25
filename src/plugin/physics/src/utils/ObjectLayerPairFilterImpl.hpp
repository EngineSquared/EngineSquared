#pragma once

#include "Layers.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Physics::Utils {
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter {
  public:
    bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case Layers::NON_MOVING: return inObject2 == Layers::MOVING || inObject2 == Layers::DEBRIS;
        case Layers::MOVING: return inObject2 != Layers::DEBRIS;     // MOVING doesn't collide with DEBRIS
        case Layers::DEBRIS: return inObject2 == Layers::NON_MOVING; // DEBRIS only collides with NON_MOVING
        default: JPH_ASSERT(false); return false;
        }
    }
};
} // namespace Physics::Utils
