#include "JoltPhysics.pch.hpp"

#include "ShutdownJoltPhysics.hpp"

#include "RigidBody3D.hpp"
#include "SoftBody3D.hpp"
#include "WheeledVehicle3D.hpp"

namespace ES::Plugin::Physics::System {
void ShutdownJoltPhysics([[maybe_unused]] ES::Engine::Core &core)
{
    // Trigger the destruction of all physics components
    core.GetRegistry().view<ES::Plugin::Physics::Component::RigidBody3D>().each(
        [&](auto entity, const auto &) {
            core.GetRegistry().remove<ES::Plugin::Physics::Component::RigidBody3D>(entity);
        }
    );

    core.GetRegistry().view<ES::Plugin::Physics::Component::SoftBody3D>().each(
        [&](auto entity, const auto &) {
            core.GetRegistry().remove<ES::Plugin::Physics::Component::SoftBody3D>(entity);
        }
    );

    core.GetRegistry().view<ES::Plugin::Physics::Component::WheeledVehicle3D>().each(
        [&](auto entity, const auto &) {
            core.GetRegistry().remove<ES::Plugin::Physics::Component::WheeledVehicle3D>(entity);
        }
    );

    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
} // namespace ES::Plugin::Physics::System
