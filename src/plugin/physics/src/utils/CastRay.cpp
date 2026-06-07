#include "utils/CastRay.hpp"
#include "resource/BodyEntityMap.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/Ray.hpp"
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/RayCast.h>

namespace Physics::Utils {
std::optional<HitRecord> CastRay(const Engine::Core &core, const Ray &ray, float maxDistance)
{

    const glm::vec3 rayDisplacement = ray.direction * maxDistance;

    JPH::RRayCast jphRay{
        JPH::RVec3{ray.origin.x,      ray.origin.y,      ray.origin.z     },
        JPH::RVec3{rayDisplacement.x, rayDisplacement.y, rayDisplacement.z},
    };
    const auto &physicsSystem = core.GetResource<Physics::Resource::PhysicsManager>().GetPhysicsSystem();
    JPH::RayCastResult hit;
    bool hitFound = physicsSystem.GetNarrowPhaseQuery().CastRay(jphRay, hit);
    if (!hitFound)
    {
        return std::nullopt;
    }
    const auto &physicsBodyToEntityMap = core.GetResource<Resource::BodyEntityMap>();
    HitRecord hitRecord{.t = hit.mFraction, .hittedEntityId = physicsBodyToEntityMap.Get(hit.mBodyID)};
    return hitRecord;
}
} // namespace Physics::Utils
