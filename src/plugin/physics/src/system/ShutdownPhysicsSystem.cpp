#include "system/ShutdownPhysicsSystem.hpp"
#include "Physics.pch.hpp"
namespace Physics::System {

void ShutdownPhysicsSystem(Engine::Core &core)
{
    if (JPH::Factory::sInstance)
    {
        JPH::Factory::sInstance->Clear();
        delete JPH::Factory::sInstance;
    }
    JPH::Factory::sInstance = nullptr;
}

} // namespace Physics::System
