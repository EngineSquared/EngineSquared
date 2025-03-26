#include "ShutdownJoltPhysics.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Core/Factory.h>
#include <Jolt/RegisterTypes.h>

namespace ES::Plugin::Physics::System {
void ShutdownJoltPhysics(ES::Engine::Core &core)
{
    JPH::UnregisterTypes();
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}
} // namespace ES::Plugin::Physics::System