#include "JoltPhysics.pch.hpp"

#include "InitJoltPhysics.hpp"

namespace ES::Plugin::Physics::System {
void InitJoltPhysics([[maybe_unused]] ES::Engine::Core &core)
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}
} // namespace ES::Plugin::Physics::System
