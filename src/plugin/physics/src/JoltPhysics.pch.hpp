// Precompiled header for the JoltPhysics plugin

// Jolt main header always needs to be included first

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

// stdlib
#include <bit>
#include <limits>
#include <memory>

// External libraries
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/RegisterTypes.h>
#include <entt/entity/entity.hpp>
#include <glm/glm.hpp>
