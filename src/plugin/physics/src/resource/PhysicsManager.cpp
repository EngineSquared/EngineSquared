#include "PhysicsManager.hpp"

#include "BroadPhaseLayerImpl.hpp"
#include "ObjectLayerPairFilterImpl.hpp"
#include "ObjectVsBroadPhaseLayerFilterImpl.hpp"

#include <limits>

namespace ES::Plugin::Physics::Resource {
PhysicsManager::PhysicsManager()
{
    _tempAllocator = std::make_shared<JPH::TempAllocatorMalloc>();
    _jobSystem = std::make_shared<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers);
    _broadPhaseLayerInterface = std::make_shared<Utils::BPLayerInterfaceImpl>();
    _objectLayerPairFilter = std::make_shared<Utils::ObjectLayerPairFilterImpl>();
    _objectVsBroadPhaseLayerFilter = std::make_shared<Utils::ObjectVsBroadPhaseLayerFilterImpl>();
    _physicsSystem = std::make_shared<JPH::PhysicsSystem>();
}

void PhysicsManager::Init()
{
    // Default values from Jolt Physics samples
    _physicsSystem->Init(10240, 0, 65536, 20480, *_broadPhaseLayerInterface, *_objectVsBroadPhaseLayerFilter,
                         *_objectLayerPairFilter);
}
} // namespace ES::Plugin::Physics::Resource
