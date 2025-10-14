#pragma once

#include "Engine.hpp"

#include "NativeScripting.hpp"

namespace NativeScripting::System {
void UpdateScripts(Engine::Core &core);

/**
 * @brief Call OnDestroy function for entity and destroy the script instance.
 *
 * IMPORTANT: This function should only be used by SetOnDestroy system.
 *
 * @param entity The entity whose script is to be destroyed.
 * @param registry The registry that manages the entity and its components.
 */
void DestroyScript(entt::registry &registry, entt::entity entity);

/**
 * @brief Sets the callback function to be called when the scripting system is destroyed.
 *
 * This function registers a callback that will be executed when the ScriptingSystem
 * is being destroyed. It allows for custom cleanup or resource deallocation.
 *
 * @param core Reference to the core engine instance.
 */
void SetOnDestroy(Engine::Core &core);
} // namespace NativeScripting::System
