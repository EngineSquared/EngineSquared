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
 * @param   parent  parent entity
 * @param   child   child entity
 */
auto SetChildOf(Engine::Entity child, Engine::Entity parent) -> void;

/**
 * Check if an entity is a child of another entity.
 *
 * @param   parent  parent entity
 * @param   child   child entity
 * @return  true if the child is a child of the parent
 */
auto IsChildOf(Engine::Entity child, Engine::Entity parent) -> bool;

/**
 * Remove the parent of an entity.
 *
 * @param   child   child entity
 */
auto RemoveParent(Engine::Entity child) -> void;

/**
 * Get the parent of an entity.
 *
 * @param   child   child entity
 * @return  the parent entity, or nullopt if the entity has no parent
 */
auto GetParent(Engine::Entity child) -> std::optional<Engine::Entity>;

/**
 * Apply a function to each child of an entity.
 *
 * @param   parent  parent entity
 * @return  no return value
 */
template <typename TFunc> auto ForEachChild(Engine::Entity parent, TFunc func) -> void
{
    const Relationship::Component::Relationship *parentRS =
        parent.TryGetComponent<Relationship::Component::Relationship>();
    if (!parentRS)
    {
        Log::Warning(fmt::format("Entity({}) has no Relationship component in ForEachChild", parent));
        return;
    }

    auto currentChildOpt = parentRS->first;
    while (currentChildOpt.has_value())
    {
        Engine::Entity currentChild = currentChildOpt.value();
        func(currentChild);
        const Relationship::Component::Relationship *currentChildRS =
            currentChild.TryGetComponent<Relationship::Component::Relationship>();
        if (!currentChildRS)
        {
            break;
        }
        currentChildOpt = currentChildRS->next;
    }
}

/**
 * Get a specific pointer list of a component for each child of an entity.
 *
 * @param   parent  parent entity
 * @return  a vector containing pointers to the specified component for each child of the parent entity
 */

template <typename TComponent>
auto GetChildComponents(Engine::Entity parent) -> std::vector<std::reference_wrapper<TComponent>>
{
    std::vector<std::reference_wrapper<TComponent>> childComponents;
    ForEachChild(parent, [&childComponents](Engine::Entity child) {
        TComponent *childComponent = child.TryGetComponent<TComponent>();
        if (childComponent)
        {
            childComponents.emplace_back(*childComponent);
        }
    });
    return childComponents;
}

/**
 * Get the pointer to a component of a parent of an Entity.
 *
 * @param   child   child entity
 * @return  the pointer to the component if the parent entity has it, or nullptr if the entity has no parent or the
 * parent has no such component
 */
template <typename TComponent> auto TryGetParentComponent(Engine::Entity child) -> TComponent *
{
    std::optional<Engine::Entity> parentOpt = GetParent(child);
    if (!parentOpt.has_value())
    {
        Log::Warning(fmt::format("Entity({}) has no parent in TryGetParentComponent", child));
        return nullptr;
    }
    Engine::Entity parent = parentOpt.value();
    return parent.TryGetComponent<TComponent>();
}
} // namespace Relationship::Utils
