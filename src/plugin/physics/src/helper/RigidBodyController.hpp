#pragma once

#include "component/RigidBody.hpp"
#include "core/Core.hpp"

namespace Physics::Helper {

// RigidBody runtime setters/getters
void SetFriction(Engine::Core &core, Engine::Entity entity, float friction);
void SetRestitution(Engine::Core &core, Engine::Entity entity, float restitution);
void SetMotionQuality(Engine::Core &core, Engine::Entity entity, Component::MotionQuality motionQuality,
                      bool useLinearCast);
void SetGravityFactor(Engine::Core &core, Engine::Entity entity, float gravityFactor);
void SetEnhancedInternalEdgeRemoval(Engine::Core &core, Engine::Entity entity, bool enabled);
void SetAxisLocks(Engine::Core &core, Engine::Entity entity, bool lockPositionX, bool lockPositionY, bool lockPositionZ,
                  bool lockRotationX, bool lockRotationY, bool lockRotationZ);

// Additional runtime setters
void SetLinearDamping(Engine::Core &core, Engine::Entity entity, float linearDamping);
void SetAngularDamping(Engine::Core &core, Engine::Entity entity, float angularDamping);
void SetMass(Engine::Core &core, Engine::Entity entity, float mass);
void SetMotionType(Engine::Core &core, Engine::Entity entity, JPH::EMotionType motionType, JPH::EActivation activation);
void SetAllowSleeping(Engine::Core &core, Engine::Entity entity, bool allowSleeping);

} // namespace Physics::Helper
