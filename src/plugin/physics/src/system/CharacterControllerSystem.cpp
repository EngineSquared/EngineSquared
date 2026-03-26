#include "system/CharacterControllerSystem.hpp"
#include "Logger.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "component/Transform.hpp"
#include "entity/Entity.hpp"
#include "exception/CharacterControllerError.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "utils/JoltConversions.hpp"
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

namespace Physics::System {

static void OnCharacterControllerConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    Engine::Entity entity{*core, entityId};

    auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
    {
        Log::Error("Cannot create CharacterController: Physics system not activated");
        return;
    }

    auto &characterController = entity.GetComponents<Component::CharacterController>();

    auto *transform = entity.TryGetComponent<Object::Component::Transform>();
    if (!transform)
    {
        Log::Warning("CharacterController added to entity without Transform - creating default Transform");
        transform = &entity.AddComponent<Object::Component::Transform>();
    }

    JPH::RefConst<JPH::Shape> shape;
    if (auto *capsule = entity.TryGetComponent<Component::CapsuleCollider>())
    {
        if (capsule->IsValid())
        {
            shape = new JPH::CapsuleShape(capsule->halfHeight, capsule->radius);
        }
        else
        {
            Log::Warning("CharacterController: CapsuleCollider is invalid, using default capsule shape");
            shape = new JPH::CapsuleShape(0.5f, 0.3f);
        }
    }
    else
    {
        shape = new JPH::CapsuleShape(0.5f, 0.3f);
    }

    JPH::CharacterVirtualSettings settings;
    settings.mMaxSlopeAngle = JPH::DegreesToRadians(characterController.maxSlopeAngle);
    settings.mShape = shape;
    settings.mMass = characterController.mass;

    auto *character = new JPH::CharacterVirtual(&settings, Utils::ToJoltRVec3(transform->GetPosition()),
                                                Utils::ToJoltQuat(transform->GetRotation()),
                                                static_cast<JPH::uint64>(entityId), &physicsManager.GetPhysicsSystem());

    entity.AddComponent<Component::CharacterControllerInternal>(character);

    Log::Debug(fmt::format("Created CharacterController for entity {}", entity));
}

static void OnCharacterControllerDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    Engine::Entity entity{*core, entityId};

    auto *internal = entity.TryGetComponent<Component::CharacterControllerInternal>();
    if (!internal || !internal->IsValid())
        return;

    internal->character = nullptr;

    Log::Debug(fmt::format("Destroyed CharacterController for entity {}", entity));

    entity.RemoveComponent<Component::CharacterControllerInternal>();
}

void CharacterControllerUpdate(Engine::Core &core)
{
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
        return;

    auto dt = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
    auto &registry = core.GetRegistry();

    auto view = registry.view<Component::CharacterController, Component::CharacterControllerInternal,
                              Object::Component::Transform>();

    view.each([&](Engine::EntityId entityId, Component::CharacterController &characterController,
                  Component::CharacterControllerInternal &characterControllerInternal,
                  Object::Component::Transform &transform) {
        if (!characterControllerInternal.IsValid())
            return;

        auto &character = *characterControllerInternal.character;

        JPH::Vec3 gravity = physicsManager.GetPhysicsSystem().GetGravity() * characterController.gravityFactor;

        JPH::Vec3 velocity = Utils::ToJoltVec3(characterController.linearVelocity);

        if (character.IsSupported())
        {
            if (velocity.GetY() < 0.0f)
                velocity.SetY(0.0f);
        }
        else
        {
            velocity.SetY(velocity.GetY() + gravity.GetY() * dt);
        }

        character.SetLinearVelocity(velocity);

        JPH::CharacterVirtual::ExtendedUpdateSettings extendedSettings;
        extendedSettings.mStickToFloorStepDown = JPH::Vec3(0.0f, -characterController.maxStepHeight, 0.0f);
        extendedSettings.mWalkStairsStepUp = JPH::Vec3(0.0f, characterController.maxStepHeight, 0.0f);
        JPH::BroadPhaseLayerFilter broadPhaseFilter;
        JPH::ObjectLayerFilter objectLayerFilter;
        JPH::BodyFilter bodyFilter;
        JPH::ShapeFilter shapeFilter;

        character.ExtendedUpdate(dt, gravity, extendedSettings, broadPhaseFilter, objectLayerFilter, bodyFilter,
                                 shapeFilter, *physicsManager.GetTempAllocator());
        characterController.linearVelocity = Utils::FromJoltVec3(character.GetLinearVelocity());

        transform.SetPosition(Utils::FromJoltRVec3(character.GetPosition()));
        transform.SetRotation(Utils::FromJoltQuat(character.GetRotation()));
    });
}

void InitCharacterControllerSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    if (!registry.ctx().contains<Engine::Core *>())
        registry.ctx().emplace<Engine::Core *>(&core);

    registry.on_construct<Component::CharacterController>().connect<&OnCharacterControllerConstruct>();
    registry.on_destroy<Component::CharacterController>().connect<&OnCharacterControllerDestroy>();
}

void ShutdownCharacterControllerSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<Component::CharacterController>().disconnect<&OnCharacterControllerConstruct>();
    registry.on_destroy<Component::CharacterController>().disconnect<&OnCharacterControllerDestroy>();
}

} // namespace Physics::System
