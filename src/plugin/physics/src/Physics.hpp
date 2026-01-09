#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

// Exceptions
#include "exception/ConstraintError.hpp"
#include "exception/RigidBodyError.hpp"

// Components
#include "component/BoxCollider.hpp"
#include "component/DefaultCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"

// Components - Constraints
#include "component/ConstraintInternal.hpp"
#include "component/ConstraintSettings.hpp"
#include "component/DistanceConstraint.hpp"
#include "component/FixedConstraint.hpp"
#include "component/PointConstraint.hpp"

// Components - Soft Body
#include "component/SoftBody.hpp"
#include "component/SoftBodyInternal.hpp"

// Resources
#include "resource/PhysicsManager.hpp"

// Systems
#include "system/ConstraintSystem.hpp"
#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/RigidBodySystem.hpp"
#include "system/SoftBodySystem.hpp"
#include "system/SyncTransformSystem.hpp"

// Helpers
#include "helper/ForceApplicator.hpp"
#include "helper/KinematicMover.hpp"
#include "helper/RigidBodyController.hpp"
#include "helper/VelocityController.hpp"

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
