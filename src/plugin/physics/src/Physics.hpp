#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

// Exceptions
#include "exception/RigidBodyError.hpp"

// Components - Colliders
#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/DefaultCollider.hpp"
#include "component/SphereCollider.hpp"

// Components - RigidBody
#include "component/RigidBody.hpp"

// Components - Constraints
#include "component/FixedConstraint.hpp"
#include "component/PointConstraint.hpp"

// Resources
#include "resource/PhysicsManager.hpp"

// Systems
#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/RigidBodySystem.hpp"
#include "system/SyncTransformSystem.hpp"

// Utils
#include "utils/BroadPhaseLayerImpl.hpp"
#include "utils/BroadPhaseLayers.hpp"
#include "utils/ContactListenerImpl.hpp"
#include "utils/JoltConversions.hpp"
#include "utils/Layers.hpp"
#include "utils/ObjectLayerPairFilterImpl.hpp"
#include "utils/ObjectVsBroadPhaseLayerFilterImpl.hpp"

// Plugin
#include "plugin/PhysicsPlugin.hpp"
