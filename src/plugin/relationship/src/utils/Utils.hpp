/**************************************************************************
 * Relationship v0.0.0
 *
 * Relationship is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file Utils.hpp
 * @brief utility functions for the Relationship component
 *
 * This file contains the declaration of the utility functions for the Relationship component.
 *
 * @author @miou-zora
 * @version 0.0.0
 * @date 18-02-2024
 **************************************************************************/

#pragma once

#include "component/Relationship.hpp"

namespace Relationship::Utils {
/**
 * Set the child of an entity to another entity.
 *
 * @param   core    core of the engine
 * @param   parent  parent entity
 * @param   child   child entity
 */
auto SetChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> void;

/**
 * Check if an entity is a child of another entity.
 *
 * @param   core    core of the engine
 * @param   parent  parent entity
 * @param   child   child entity
 * @return  true if the child is a child of the parent
 */
auto IsChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> bool;

/**
 * Remove the parent of an entity.
 *
 * @param   core    core of the engine
 * @param   child   child entity
 */
auto RemoveParent(Engine::Core &core, Engine::Entity child) -> void;

/**
 * Get the parent of an entity.
 *
 * @param   core    core of the engine
 * @param   child   child entity
 * @return  parent entity
 */
auto GetParent(Engine::Core &core, Engine::Entity child) -> Engine::Entity;
} // namespace Relationship::Utils
