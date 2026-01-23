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

static void OnFixedConstraintConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
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

static void OnFixedConstraintDestroy(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    DestroyConstraint(registry, entity, "FixedConstraint");
}

//=============================================================================
// Distance Constraint Handler
//=============================================================================

static void OnDistanceConstraintConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    constexpr const char *NAME = "DistanceConstraint";

    CreateConstraintGeneric<Component::ConstraintType::Distance, Component::DistanceConstraint,
                            JPH::DistanceConstraintSettings>(
        registry, entity, NAME,
        [](auto &constraint, auto &joltSettings) {
            joltSettings.mSpace = JPH::EConstraintSpace::LocalToBodyCOM;
            joltSettings.mPoint1 = Utils::ToJoltRVec3(constraint.localPointA);
            joltSettings.mPoint2 = Utils::ToJoltRVec3(constraint.localPointB);
            joltSettings.mMinDistance = constraint.minDistance;
            joltSettings.mMaxDistance = constraint.maxDistance;
            if (!constraint.settings.IsRigid())
            {
                joltSettings.mLimitsSpringSettings.mMode = JPH::ESpringMode::StiffnessAndDamping;
                joltSettings.mLimitsSpringSettings.mStiffness = constraint.settings.stiffness * 10000.0f;
                joltSettings.mLimitsSpringSettings.mDamping = constraint.settings.damping * 100.0f;
            }
        },
        [](auto const &constraint) {
            if (constraint.maxDistance >= 0 && constraint.minDistance >= 0 &&
                constraint.maxDistance < constraint.minDistance)
            {
                Log::Error(fmt::format("{}: maxDistance < minDistance", "DistanceConstraint"));
                return false;
            }
            return true;
        });
}

static void OnDistanceConstraintDestroy(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    DestroyConstraint(registry, entity, "DistanceConstraint");
}

//=============================================================================
// Point Constraint Handler
//=============================================================================

static void OnPointConstraintConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
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

static void OnPointConstraintDestroy(Engine::Core::Registry &registry, Engine::EntityId entity)
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

    Log::Info("ConstraintSystem initialized");
}

} // namespace Physics::System
