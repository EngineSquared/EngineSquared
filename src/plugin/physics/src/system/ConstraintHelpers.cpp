#include "ConstraintHelpers.hpp"

namespace Physics::System {

std::optional<ConstraintContext> ConstraintContext::Create(entt::registry &registry, const char *constraintName)
{
    auto **corePtr = registry.ctx().find<Engine::Core *>();
    if (!corePtr || !*corePtr)
    {
        Log::Error(fmt::format("Cannot create {}: Engine::Core not available", constraintName));
        return std::nullopt;
    }
    auto &coreRef = **corePtr;

    auto &physicsManagerRef = coreRef.GetResource<Resource::PhysicsManager>();

    if (!physicsManagerRef.IsPhysicsActivated())
    {
        Log::Error(fmt::format("Cannot create {}: Physics system not activated", constraintName));
        return std::nullopt;
    }

    return ConstraintContext{coreRef, registry, physicsManagerRef, physicsManagerRef.GetPhysicsSystem()};
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

    ctx.registry.emplace_or_replace<Component::ConstraintInternal>(entity, joltConstraint, type, settings.breakForce,
                                                                   settings.breakTorque);
    try
    {
        ctx.physicsSystem.AddConstraint(joltConstraint);
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Error(fmt::format("{}: Failed to register constraint: {}", constraintName, e.what()));
        ctx.registry.remove<Component::ConstraintInternal>(entity);
        return;
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{}: Failed to register constraint: {}", constraintName, e.what()));
        ctx.registry.remove<Component::ConstraintInternal>(entity);
        return;
    }

    Log::Debug(fmt::format("Created {} for entity {}", constraintName, static_cast<uint32_t>(entity)));
}

void DestroyConstraint(entt::registry &registry, entt::entity entity, const char *constraintName)
{
    try
    {
        auto **corePtr = registry.ctx().find<Engine::Core *>();
        if (!corePtr || !*corePtr)
            return;
        auto &coreRef = **corePtr;
        auto &physicsManagerRef = coreRef.GetResource<Resource::PhysicsManager>();

        if (!physicsManagerRef.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::ConstraintInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        physicsManagerRef.GetPhysicsSystem().RemoveConstraint(internal->constraint);

        Log::Debug(fmt::format("Destroyed {} for entity {}", constraintName, entt::to_integral(entity)));

        registry.remove<Component::ConstraintInternal>(entity);
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Warn(fmt::format("{} destroy constraint error: {}", constraintName, e.what()));
    }
    catch (const std::bad_alloc &e)
    {
        Log::Critical(fmt::format("{} destroy memory error: {}", constraintName, e.what()));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{} destroy unexpected error: {}", constraintName, e.what()));
    }
}

} // namespace Physics::System
