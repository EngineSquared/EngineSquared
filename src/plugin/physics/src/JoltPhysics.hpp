#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

// Components
#include "component/RigidBody3D.hpp"
#include "component/SoftBody3D.hpp"
#include "component/WheeledVehicle3D.hpp"

// Exceptions
#include "exception/WheeledVehicleBuilderError.hpp"

// Resource
#include "resource/PhysicsManager.hpp"

// Systems
#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/PhysicsUpdateRigidBody.hpp"
#include "system/PhysicsUpdateSoftBody.hpp"
#include "system/PhysicsUpdateWheeledVehicle.hpp"
#include "system/ShutdownJoltPhysics.hpp"

// Utils
#include "utils/BroadPhaseLayerImpl.hpp"
#include "utils/BroadPhaseLayers.hpp"
#include "utils/ContactListenerImpl.hpp"
#include "utils/Layers.hpp"
#include "utils/ObjectLayerPairFilterImpl.hpp"
#include "utils/ObjectVsBroadPhaseLayerFilterImpl.hpp"
#include "utils/SoftBodyVertexSettings.hpp"
#include "utils/WheeledVehicleBuilder.hpp"

// Plugin
#include "plugin/PluginPhysics.hpp"
