#include "Physics.pch.hpp"

#include "system/ConstraintSystem.hpp"

#include "Logger.hpp"
#include "component/ConstraintInternal.hpp"
#include "component/ConstraintSettings.hpp"
#include "component/DistanceConstraint.hpp"
#include "component/FixedConstraint.hpp"
#include "component/PointConstraint.hpp"
#include "component/RigidBodyInternal.hpp"
#include "exception/ConstraintError.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

// Jolt constraint headers
#include <Jolt/Physics/Body/BodyLockMulti.h>
#include <Jolt/Physics/Constraints/DistanceConstraint.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Physics/Constraints/PointConstraint.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "system/ConstraintHelpers.hpp"

namespace Physics::System {

//=============================================================================
// Fixed Constraint Handler
//=============================================================================

static void OnFixedConstraintConstruct(entt::registry &registry, entt::entity entity)
{
    constexpr const char *NAME = "FixedConstraint";

    CreateConstraintGeneric<Component::ConstraintType::Fixed, Component::FixedConstraint, JPH::FixedConstraintSettings>(
        registry, entity, NAME,
        [](auto &constraint, auto &joltSettings) {
            joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
            joltSettings.mAutoDetectPoint = false;
            joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);
        },
        [](auto const &) { return true; });
}

static void OnDistanceConstraintDestroy(entt::registry &registry, entt::entity entity)
{
    DestroyConstraint(registry, entity, "DistanceConstraint");
}

//=============================================================================
// Point Constraint Handler
//=============================================================================

static void OnPointConstraintConstruct(entt::registry &registry, entt::entity entity)
{
    constexpr const char *NAME = "PointConstraint";

    CreateConstraintGeneric<Component::ConstraintType::Point, Component::PointConstraint, JPH::PointConstraintSettings>(
        registry, entity, NAME,
        [](auto &constraint, auto &joltSettings) {
            joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
            joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);
        },
        [](auto const &) { return true; });
}

static void OnPointConstraintDestroy(entt::registry &registry, entt::entity entity)
{
    DestroyConstraint(registry, entity, "PointConstraint");
}

//=============================================================================
// Public System Function
//=============================================================================

void InitConstraintSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    if (!registry.ctx().contains<Engine::Core *>())
        registry.ctx().emplace<Engine::Core *>(&core);

    struct ConstraintSystemInitializedTag {};
    if (registry.ctx().contains<ConstraintSystemInitializedTag>())
        return;
    registry.ctx().emplace<ConstraintSystemInitializedTag>();

    // Register constraint hooks
    registry.on_construct<Component::FixedConstraint>().connect<&OnFixedConstraintConstruct>();
    registry.on_destroy<Component::FixedConstraint>().connect<&OnFixedConstraintDestroy>();

    registry.on_construct<Component::DistanceConstraint>().connect<&OnDistanceConstraintConstruct>();
    registry.on_destroy<Component::DistanceConstraint>().connect<&OnDistanceConstraintDestroy>();

    registry.on_construct<Component::PointConstraint>().connect<&OnPointConstraintConstruct>();
    registry.on_destroy<Component::PointConstraint>().connect<&OnPointConstraintDestroy>();

    registry.ctx().emplace<ConstraintSystemInitializedTag>();

    Log::Info("ConstraintSystem initialized");
}

} // namespace Physics::System
