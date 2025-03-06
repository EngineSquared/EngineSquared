#pragma once

#include "BroadPhaseLayers.hpp"
#include "Layers.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace ES::Plugin::Physics::Utils {
// BroadPhaseLayerInterface implementation
// This defines a mapping between object and broadphase layers.
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface {
  public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        _objectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        _objectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override { return BroadPhaseLayers::NUM_LAYERS; }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return _objectToBroadPhase[inLayer];
    }

    virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type) inLayer)
        {
        case (JPH::BroadPhaseLayer::Type) BroadPhaseLayers::NON_MOVING: return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type) BroadPhaseLayers::MOVING: return "MOVING";
        default: JPH_ASSERT(false); return "INVALID";
        }
    }

  private:
    JPH::BroadPhaseLayer _objectToBroadPhase[Layers::NUM_LAYERS];
};
} // namespace ES::Plugin::Physics::Utils
