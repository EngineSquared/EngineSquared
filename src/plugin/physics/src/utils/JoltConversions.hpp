/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file JoltConversions.hpp
 * @brief Conversion utilities between GLM and Jolt Physics types
 *
 * This file provides conversion functions between GLM types (vec3, quat) and
 * Jolt Physics types (Vec3, Quat). These inline functions ensure zero-cost
 * conversions for efficient interoperability.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-30
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Physics::Utils {

//=============================================================================
// GLM → Jolt conversions
//=============================================================================

/**
 * @brief Convert glm::vec3 to JPH::Vec3
 */
inline JPH::Vec3 ToJoltVec3(const glm::vec3 &v) { return JPH::Vec3(v.x, v.y, v.z); }

/**
 * @brief Convert glm::quat to JPH::Quat
 */
inline JPH::Quat ToJoltQuat(const glm::quat &q) { return JPH::Quat(q.x, q.y, q.z, q.w); }

//=============================================================================
// Jolt → GLM conversions
//=============================================================================

/**
 * @brief Convert JPH::Vec3 to glm::vec3
 */
inline glm::vec3 FromJoltVec3(const JPH::Vec3 &v) { return glm::vec3(v.GetX(), v.GetY(), v.GetZ()); }

/**
 * @brief Convert JPH::RVec3 (real vector, double precision) to glm::vec3
 *
 * @note RVec3 is used for positions in Jolt to support large worlds.
 * Conversion to float vec3 is safe for most game worlds.
 */
inline glm::vec3 FromJoltRVec3(const JPH::RVec3 &v)
{
    return glm::vec3(static_cast<float>(v.GetX()), static_cast<float>(v.GetY()), static_cast<float>(v.GetZ()));
}

/**
 * @brief Convert JPH::Quat to glm::quat
 */
inline glm::quat FromJoltQuat(const JPH::Quat &q) { return glm::quat(q.GetW(), q.GetX(), q.GetY(), q.GetZ()); }

} // namespace Physics::Utils
