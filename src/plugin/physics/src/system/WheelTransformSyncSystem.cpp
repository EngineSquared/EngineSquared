#include "Physics.pch.hpp"

#include "system/WheelTransformSyncSystem.hpp"

#include "component/VehicleInternal.hpp"
#include "utils/JoltConversions.hpp"

#include "Object.hpp"

#include <Jolt/Physics/Vehicle/VehicleConstraint.h>

namespace Physics::System {

void WheelTransformSyncSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.view<Component::VehicleInternal>().each([&](const Component::VehicleInternal &internal) {
        if (!internal.IsValid() || !internal.vehicleConstraint)
            return;

        JPH::VehicleConstraint *constraint = internal.vehicleConstraint;

        for (size_t i = 0; i < 4; ++i)
        {
            Engine::Entity wheelEntity{core, internal.wheelEntities[i]};
            if (wheelEntity.IsValid() == false)
                continue;

            auto *wheelTransform = wheelEntity.TryGetComponent<Object::Component::Transform>();
            if (!wheelTransform)
                continue;

            JPH::RMat44 wheelWorldTransform =
                constraint->GetWheelWorldTransform(static_cast<uint32_t>(i), JPH::Vec3::sAxisY(), JPH::Vec3::sAxisZ());

            JPH::Quat meshRotation = JPH::Quat::sRotation(JPH::Vec3::sAxisZ(), JPH::DegreesToRadians(90.0f));
            JPH::Quat finalRot = wheelWorldTransform.GetQuaternion() * meshRotation;

            wheelTransform->SetPosition(Utils::FromJoltVec3(wheelWorldTransform.GetTranslation()));
            wheelTransform->SetRotation(Utils::FromJoltQuat(finalRot));
        }
    });
}

} // namespace Physics::System
