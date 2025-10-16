#include "Physics.pch.hpp"

#include "system/InitJoltPhysics.hpp"

namespace Physics::System {
void InitJoltPhysics([[maybe_unused]] Engine::Core &core)
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}
} // namespace Physics::System
