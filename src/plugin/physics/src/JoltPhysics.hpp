#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include "component/RigidBody3D.hpp"

#include "resource/PhysicsManager.hpp"

#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/ShutdownJoltPhysics.hpp"

#include "utils/BroadPhaseLayerImpl.hpp"
#include "utils/BroadPhaseLayers.hpp"
#include "utils/Layers.hpp"
#include "utils/ObjectLayerPairFilterImpl.hpp"
#include "utils/ObjectVsBroadPhaseLayerFilterImpl.hpp"

#include "plugin/PluginPhysics.hpp"
