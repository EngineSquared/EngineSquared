#include "Physics.pch.hpp"

#include "system/SyncTransformSystem.hpp"

#include "Logger.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"

#include "Object.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>

namespace Physics::System {

void SyncTransformWithPhysics(Engine::Core &core)
{
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();
    auto &registry = core.GetRegistry();

    auto view = registry.view<Component::RigidBody, Component::RigidBodyInternal, Object::Component::Transform>();

    for (auto entity : view)
    {
        auto &rigidBody = view.get<Component::RigidBody>(entity);
        auto &internal = view.get<Component::RigidBodyInternal>(entity);
        auto &transform = view.get<Object::Component::Transform>(entity);

        if (rigidBody.motionType == Component::MotionType::Static)
            continue;

        if (!internal.IsValid())
            continue;

        JPH::RVec3 joltPosition = bodyInterface.GetCenterOfMassPosition(internal.bodyID);
        JPH::Quat joltRotation = bodyInterface.GetRotation(internal.bodyID);

        transform.SetPosition(Utils::FromJoltRVec3(joltPosition));
        transform.SetRotation(Utils::FromJoltQuat(joltRotation));
    }
}

} // namespace Physics::System
