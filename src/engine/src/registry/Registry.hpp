#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <unordered_map>

#include "Clock.hpp"

#include "ScheduleLabel.hpp"

namespace ES::Engine {
/**
 * The registry is used to create, kill and manage entities.
 * It is also used to register and get components. These registered components can be used by systems.
 * After registering all the components and systems, the user can call the RunSystems() method to run all the systems.
 * To use ECS, you need to create a registry and register all the components and systems you need and after that
 * you can create entities and add components to them.
 */
class Registry {
  private:
    using USystem = std::function<void(Registry &)>;

  public:
    Registry();
    ~Registry() = default;

    /**
     * Get the entt::registry that contains all components.
     * It should be used to update component through systems.
     *
     * @return  registry that contains all components.
     */
    inline entt::registry &GetRegistry() { return *_registry; }
    /**
     * Create an entity.
     *
     * @return  The entity created.
     */
    entt::entity CreateEntity();

    /**
     * Store a resource instance.
     * Resources are unique struct or class (like a singleton) that contains global informations.
     * Example: AssetsManager, TimeProvider, WindowResource, SceneManager, InputManager or NetworkManager.
     *
     * @tparam  TResource   type of the resource to add
     * @param   resource    rvalue of the resource to add
     * @return  reference of the added resource
     */
    template <typename TResource> inline TResource &RegisterResource(TResource &&resource);

    /**
     * Get a reference's resource.
     * Resources are unique struct or class (like a singleton) that contains global informations.
     * Example: AssetsManager, TimeProvider, WindowResource, SceneManager, InputManager or NetworkManager.
     *
     * @tparam  TResource   type of the resource to get
     * @return  reference of the resource
     */
    template <typename TResource> inline TResource &GetResource();

    /**
     * Add system to the registry. A system is a function that will be called by the registry.
     * The function must take a Registry as first parameter.
     * The function must return void.
     * The function will be called by the registry in the order they were added.
     * If the label is FIXED, the system will be called at a fixed rate and you will need to call GetFixedDeltaTime to
     * get the delta time.
     *
     * @param   f   The function to add.
     * @param   label   The label of the schedule. It can be NON_FIXED for systems that don't need to be called at a
     * fixed rate or FIXED for systems that need to be called at a fixed rate.
     * @see ScheduleLabel
     * @see GetFixedDeltaTime
     */
    void RegisterSystem(USystem const &f, ES::Engine::ScheduleLabel label = ES::Engine::ScheduleLabel::NON_FIXED);

    /**
     * Get the fixed delta time for the fixed systems.
     *
     * @return  the fixed delta time
     */
    float GetFixedDeltaTime() const { return _fixedUpdateclock.GetTickRate(); }

    /**
     * Run all the systems. The systems will be called in the order they were added. It will also update the delta time.
     */
    void RunSystems();

    /**
     * Check if entity is valid in the context of the registry. It check if the id of the entity exist.
     */
    bool IsEntityValid(entt::entity entity);

    /**
     * Clear all entities and components from the registry.
     */
    void ClearEntities();

  private:
    std::unique_ptr<entt::registry> _registry;
    std::unordered_map<ES::Engine::ScheduleLabel, std::vector<USystem>> _systems;
    ES::Utils::Clock _fixedUpdateclock;
};
} // namespace ES::Engine

#include "Registry.inl"
