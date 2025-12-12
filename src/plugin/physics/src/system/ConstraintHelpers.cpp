#include "ConstraintHelpers.hpp"

namespace Physics::System {

std::optional<ConstraintContext> ConstraintContext::Create(entt::registry &registry, const char *constraintName)
{
    auto &core = *registry.ctx().get<Engine::Core *>();
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
    {
        Log::Error(fmt::format("Cannot create {}: Physics system not activated", constraintName));
        return std::nullopt;
    }

    return ConstraintContext{core, registry, physicsManager, physicsManager.GetPhysicsSystem()};
}

Component::RigidBodyInternal *GetBodyInternal(entt::registry &registry, Engine::Entity entity,
                                              const char *constraintName, const char *bodyName)
{
    auto *internal = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(entity));
    if (!internal || !internal->IsValid())
    {
        Log::Error(fmt::format("{}: {} has no valid RigidBodyInternal", constraintName, bodyName));
        return nullptr;
    }
    return internal;
}

void FinalizeConstraint(ConstraintContext &ctx, entt::entity entity, JPH::Constraint *joltConstraint,
                        Component::ConstraintType type, const Component::ConstraintSettings &settings,
                        const char *constraintName)
{
    if (!joltConstraint)
    {
        Log::Error(fmt::format("{}: Failed to create Jolt constraint", constraintName));
        return;
    }

    ctx.physicsSystem.AddConstraint(joltConstraint);
    ctx.registry.emplace<Component::ConstraintInternal>(entity, joltConstraint, type, settings.breakForce,
                                                        settings.breakTorque);

    Log::Debug(fmt::format("Created {} for entity {}", constraintName, static_cast<uint32_t>(entity)));
}

void DestroyConstraint(entt::registry &registry, entt::entity entity, const char *constraintName)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();
        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::ConstraintInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        physicsManager.GetPhysicsSystem().RemoveConstraint(internal->constraint);

        Log::Debug(fmt::format("Destroyed {} for entity {}", constraintName, static_cast<uint32_t>(entity)));

        registry.remove<Component::ConstraintInternal>(entity);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{} destroy error: {}", constraintName, e.what()));
    }
}

} // namespace Physics::System
