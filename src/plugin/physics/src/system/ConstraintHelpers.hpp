#pragma once

#include "Physics.pch.hpp"

#include "Logger.hpp"
#include "component/ConstraintInternal.hpp"
#include "component/ConstraintSettings.hpp"
#include "component/RigidBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"

#include "exception/ConstraintError.hpp"
#include <array>
#include <fmt/format.h>
#include <stdexcept>

#include <Jolt/Physics/Body/BodyLockMulti.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

namespace Physics::System {

struct ConstraintContext {
    Engine::Core &core;
    entt::registry &registry;
    Resource::PhysicsManager &physicsManager;
    JPH::PhysicsSystem &physicsSystem;

    static std::optional<ConstraintContext> Create(entt::registry &registry, const char *constraintName);
};

template <typename ConstraintT>
static bool ValidateConstraint(const ConstraintT &constraint, const char *constraintName)
{
    if (!constraint.bodyA.IsValid())
    {
        Log::Error(fmt::format("{}: bodyA is invalid", constraintName));
        return false;
    }

    if (constraint.bodyA == constraint.bodyB)
    {
        Log::Error(fmt::format("{}: Cannot constrain body to itself", constraintName));
        return false;
    }

    return true;
}

Component::RigidBodyInternal *GetBodyInternal(entt::registry &registry, Engine::Entity entity,
                                              const char *constraintName, const char *bodyName);

template <typename SettingsT, typename ConstraintT>
static JPH::Constraint *CreateJoltConstraint(ConstraintContext &ctx, SettingsT &joltSettings,
                                             const ConstraintT &constraint, Component::RigidBodyInternal *internalA,
                                             const char *constraintName)
{
    if (constraint.IsWorldConstraint())
    {
        JPH::BodyLockWrite lockA(ctx.physicsSystem.GetBodyLockInterface(), internalA->bodyID);
        if (!lockA.Succeeded())
        {
            Log::Error(fmt::format("{}: Failed to lock bodyA", constraintName));
            return nullptr;
        }

        joltSettings.mSpace = JPH::EConstraintSpace::WorldSpace;
        joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);

        return joltSettings.Create(lockA.GetBody(), JPH::Body::sFixedToWorld);
    }

    auto *internalB = GetBodyInternal(ctx.registry, constraint.bodyB, constraintName, "bodyB");
    if (!internalB)
        return nullptr;

    std::array<JPH::BodyID, 2> bodyIDs = {internalA->bodyID, internalB->bodyID};
    JPH::BodyLockMultiWrite lock(ctx.physicsSystem.GetBodyLockInterface(), bodyIDs, 2);

    JPH::Body *bodyA = lock.GetBody(0);
    JPH::Body *bodyB = lock.GetBody(1);

    if (!bodyA || !bodyB)
    {
        Log::Error(fmt::format("{}: Failed to lock bodies", constraintName));
        return nullptr;
    }

    return joltSettings.Create(*bodyA, *bodyB);
}

void FinalizeConstraint(ConstraintContext &ctx, entt::entity entity, JPH::Constraint *joltConstraint,
                        Component::ConstraintType type, const Component::ConstraintSettings &settings,
                        const char *constraintName);

void DestroyConstraint(entt::registry &registry, entt::entity entity, const char *constraintName);

template <Component::ConstraintType TYPE, typename CompT, typename SettingsT, typename Configurator,
          typename ExtraValidate>
static void CreateConstraintGeneric(entt::registry &registry, entt::entity entity, const char *constraintName,
                                    Configurator &&configurator, ExtraValidate &&extraValidate)
{
    try
    {
        auto ctxOpt = ConstraintContext::Create(registry, constraintName);
        if (!ctxOpt)
            return;
        auto &ctx = *ctxOpt;

        auto &constraint = registry.get<CompT>(entity);

        if (!ValidateConstraint(constraint, constraintName))
            return;

        if (!extraValidate(constraint))
            return;

        auto *internalA = GetBodyInternal(ctx.registry, constraint.bodyA, constraintName, "bodyA");
        if (!internalA)
            return;

        SettingsT joltSettings;
        joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;

        configurator(constraint, joltSettings);

        auto *joltConstraint = CreateJoltConstraint(ctx, joltSettings, constraint, internalA, constraintName);
        FinalizeConstraint(ctx, entity, joltConstraint, TYPE, constraint.settings, constraintName);
    }
    catch (const Physics::Exception::ConstraintError &e)
    {
        Log::Warn(fmt::format("{} constraint error: {}", constraintName, e.what()));
    }
    catch (const std::bad_alloc &e)
    {
        Log::Critical(fmt::format("{} bad alloc: {}", constraintName, e.what()));
    }
}

} // namespace Physics::System
