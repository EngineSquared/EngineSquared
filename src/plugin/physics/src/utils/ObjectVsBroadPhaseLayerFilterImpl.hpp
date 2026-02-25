#pragma once

#include "BroadPhaseLayers.hpp"
#include "Layers.hpp"

namespace Physics::Utils {
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter {
  public:
    bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING: return inLayer2 == BroadPhaseLayers::MOVING || inLayer2 == BroadPhaseLayers::DEBRIS;
        case Layers::MOVING: return inLayer2 != BroadPhaseLayers::DEBRIS; // MOVING doesn't collide with DEBRIS
        case Layers::DEBRIS: return inLayer2 == BroadPhaseLayers::NON_MOVING; // DEBRIS only collides with NON_MOVING
        default: JPH_ASSERT(false); return false;
        }
    }
};
} // namespace Physics::Utils
