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
 * @file Relationship.hpp
 * @brief component to manage link between childs and parent
 *
 * This file contains the declaration of the Relationship component.
 *
 * @author @miou-zora
 * @version 0.0.0
 * @date 18-02-2024
 **************************************************************************/

#pragma once

#include "entity/Entity.hpp"

namespace Relationship::Component {
/**
 * Component that represents a relationship between entities
 */
struct Relationship {
    // @brief number of children of the entity
    std::size_t children = 0;
    // @brief first child of the entity
    Engine::Entity first = Engine::Entity::entity_null_id;
    // @brief previous sibling of the entity
    Engine::Entity prev = Engine::Entity::entity_null_id;
    // @brief next sibling of the entity
    Engine::Entity next = Engine::Entity::entity_null_id;
    // @brief parent of the entity
    Engine::Entity parent = Engine::Entity::entity_null_id;
};
} // namespace Relationship::Component
