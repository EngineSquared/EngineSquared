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
 * @file ShapeGenerator.hpp
 * @brief Utility functions to generate basic geometric shapes
 *
 * This file provides helper functions to create common 3D shapes like cubes,
 * spheres, planes, etc. with proper vertices, normals, texture coordinates,
 * and indices.
 *
 * Note: for compile efficiency, it is recommended to include only the specific
 * shape generator headers you need instead of this umbrella header.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"

#include <glm/glm.hpp>

#include "BoxGenerator.hpp"
#include "CapsuleGenerator.hpp"
#include "ClothGenerator.hpp"
#include "CubeGenerator.hpp"
#include "CylinderGenerator.hpp"
#include "JellyCubeGenerator.hpp"
#include "PlaneGenerator.hpp"
#include "RopeGenerator.hpp"
#include "SphereGenerator.hpp"
#include "WheelGenerator.hpp"
