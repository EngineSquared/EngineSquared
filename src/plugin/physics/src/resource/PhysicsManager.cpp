#include "JoltPhysics.pch.hpp"

#include "PhysicsManager.hpp"

#include "utils/BroadPhaseLayerImpl.hpp"
#include "utils/ContactListenerImpl.hpp"
#include "utils/ObjectLayerPairFilterImpl.hpp"
#include "utils/ObjectVsBroadPhaseLayerFilterImpl.hpp"

namespace Plugin::Physics::Resource {
PhysicsManager::PhysicsManager()
{
    _tempAllocator = std::make_shared<JPH::TempAllocatorMalloc>();
    _jobSystem = std::make_shared<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers);
    _broadPhaseLayerInterface = std::make_shared<Utils::BPLayerInterfaceImpl>();
    _objectLayerPairFilter = std::make_shared<Utils::ObjectLayerPairFilterImpl>();
    _objectVsBroadPhaseLayerFilter = std::make_shared<Utils::ObjectVsBroadPhaseLayerFilterImpl>();
    _physicsSystem = std::make_shared<JPH::PhysicsSystem>();
    _contactListener = nullptr;
}

void PhysicsManager::Init(Engine::Core &core)
{
    // Default values from Jolt Physics samples
    _physicsSystem->Init(10240, 0, 65536, 20480, *_broadPhaseLayerInterface, *_objectVsBroadPhaseLayerFilter,
                         *_objectLayerPairFilter);
    _contactListener = std::make_shared<Utils::ContactListenerImpl>(core);
    _physicsSystem->SetContactListener(_contactListener.get());
}
} // namespace Plugin::Physics::Resource
