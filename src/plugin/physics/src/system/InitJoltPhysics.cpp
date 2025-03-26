#include "InitJoltPhysics.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace ES::Plugin::Physics::System {
void InitJoltPhysics(ES::Engine::Core &core)
{
    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();
}
} // namespace ES::Plugin::Physics::System