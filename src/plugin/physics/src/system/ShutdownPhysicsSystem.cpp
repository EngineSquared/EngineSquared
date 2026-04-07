#include "system/ShutdownPhysicsSystem.hpp"
namespace Physics::System {

void ShutdownPhysicsSystem(Engine::Core &core)
{
    JPH::Factory::sInstance->Clear();
    delete JPH::Factory::sInstance;
}

} // namespace Physics::System
