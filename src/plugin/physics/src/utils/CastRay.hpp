#pragma once

#include "utils/HitRecord.hpp"
#include <glm/vec3.hpp>
#include <optional>

namespace Engine {
class Core;
}

namespace Physics::Utils {
class Ray;
std::optional<HitRecord> CastRay(const Engine::Core &core, const Ray &ray, float maxDistance);
} // namespace Physics::Utils
