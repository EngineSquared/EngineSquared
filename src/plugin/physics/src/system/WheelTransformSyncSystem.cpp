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

        auto *constraint = internal.vehicleConstraint;

        for (size_t i = 0; i < 4; ++i)
        {
            Engine::Entity wheelEntity = internal.wheelEntities[i];
            if (!wheelEntity.IsValid())
                continue;

            auto *wheelTransform = registry.try_get<Object::Component::Transform>(wheelEntity);
            if (!wheelTransform)
                continue;

            JPH::RMat44 wheelWorldTransform =
                constraint->GetWheelWorldTransform(static_cast<uint32_t>(i), JPH::Vec3::sAxisY(), JPH::Vec3::sAxisX());

            JPH::Vec3 wheelPos = wheelWorldTransform.GetTranslation();
            JPH::Quat wheelRot = wheelWorldTransform.GetQuaternion();

            wheelTransform->SetPosition(Utils::FromJoltVec3(wheelPos));
            wheelTransform->SetRotation(Utils::FromJoltQuat(wheelRot));
        }
    });
}

} // namespace Physics::System
