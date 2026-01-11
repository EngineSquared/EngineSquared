#include "ConstraintHelpers.hpp"

namespace Physics::System {

std::optional<ConstraintContext> ConstraintContext::Create(entt::registry &registry, const char *constraintName)
{
    const char *safeName = constraintName ? constraintName : "<constraint>";
    auto **corePtr = registry.ctx().find<Engine::Core *>();
    if (!corePtr || !*corePtr)
    {
        Log::Error(fmt::format("Cannot create {}: Engine::Core not available", safeName));
        return std::nullopt;
    }
    auto &coreRef = **corePtr;

    Resource::PhysicsManager *physicsManagerPtr = nullptr;
    try
    {
        physicsManagerPtr = &coreRef.GetResource<Resource::PhysicsManager>();
    }
    catch (const Engine::Exception::MissingResourceError &e)
    {
        Log::Error(fmt::format("Cannot create {}: PhysicsManager resource not registered: {}", safeName, e.what()));
        return std::nullopt;
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("Cannot create {}: Failed to access PhysicsManager: {}", safeName, e.what()));
        return std::nullopt;
    }
    auto &physicsManagerRef = *physicsManagerPtr;

    if (!physicsManagerRef.IsPhysicsActivated())
    {
        Log::Error(fmt::format("Cannot create {}: Physics system not activated", safeName));
        return std::nullopt;
    }

    return ConstraintContext{coreRef, registry, physicsManagerRef, physicsManagerRef.GetPhysicsSystem()};
}

Component::RigidBodyInternal *GetBodyInternal(entt::registry &registry, Engine::Entity entity,
                                              const char *constraintName, const char *bodyName)
{
    const char *safeName = constraintName ? constraintName : "<constraint>";
    auto *internal = registry.try_get<Component::RigidBodyInternal>(static_cast<entt::entity>(entity));
    if (!internal || !internal->IsValid())
    {
        Log::Error(fmt::format("{}: {} has no valid RigidBodyInternal", safeName, bodyName));
        return nullptr;
    }
    return internal;
}

void FinalizeConstraint(ConstraintContext &ctx, entt::entity entity, JPH::Constraint *joltConstraint,
                        Component::ConstraintType type, const Component::ConstraintSettings &settings,
                        const char *constraintName)
{
    const char *safeName = constraintName ? constraintName : "<constraint>";
    if (!joltConstraint)
    {
        Log::Error(fmt::format("{}: Failed to create Jolt constraint", safeName));
        return;
    }

    try
    {
        ctx.physicsSystem.AddConstraint(joltConstraint);
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Error(fmt::format("{}: Failed to register constraint: {}", safeName, e.what()));
        joltConstraint->Release();
        return;
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{}: Failed to register constraint: {}", safeName, e.what()));
        joltConstraint->Release();
        return;
    }

    try
    {
        if (auto *existing = ctx.registry.try_get<Component::ConstraintInternal>(entity);
            existing && existing->IsValid())
        {
            ctx.physicsSystem.RemoveConstraint(existing->constraint);
            ctx.registry.remove<Component::ConstraintInternal>(entity);
        }

        ctx.registry.emplace_or_replace<Component::ConstraintInternal>(entity, joltConstraint, type,
                                                                       settings.breakForce, settings.breakTorque);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{}: Failed to store ConstraintInternal: {}", safeName, e.what()));
        try
        {
            ctx.physicsSystem.RemoveConstraint(joltConstraint);
        }
        catch (...)
        {
        }
        joltConstraint->Release();
        return;
    }

    Log::Debug(fmt::format("Created {} for entity {}", safeName, entt::to_integral(entity)));
}

void DestroyConstraint(entt::registry &registry, entt::entity entity, const char *constraintName)
{
    const char *safeName = constraintName ? constraintName : "<constraint>";
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

        auto *constraint = internal->constraint;
        physicsManagerRef.GetPhysicsSystem().RemoveConstraint(constraint);
        registry.remove<Component::ConstraintInternal>(entity);

        Log::Debug(fmt::format("Destroyed {} for entity {}", safeName, entt::to_integral(entity)));
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Warn(fmt::format("{} destroy constraint error: {}", safeName, e.what()));
    }
    catch (const std::bad_alloc &e)
    {
        Log::Critical(fmt::format("{} destroy memory error: {}", safeName, e.what()));
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Error(fmt::format("{} destroy unexpected error: {}", safeName, e.what()));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("{} destroy unexpected error: {}", constraintName, e.what()));
    }
}

} // namespace Physics::System
