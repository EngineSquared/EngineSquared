#include "JoltPhysics.pch.hpp"

#include "ShutdownJoltPhysics.hpp"

namespace ES::Plugin::Physics::System {
void ShutdownJoltPhysics([[maybe_unused]] ES::Engine::Core &core)
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
} // namespace ES::Plugin::Physics::System
