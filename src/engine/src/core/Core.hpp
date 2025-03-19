#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "SchedulerContainer.hpp"
#include "Update.hpp"

namespace ES::Engine {
/**
 * The registry is used to create, kill and manage entities.
 * It is also used to register and get components. These registered components can be used by systems.
 * After registering all the components and systems, the user can call the RunSystems() method to run all the systems.
 * To use ECS, you need to create a registry and register all the components and systems you need and after that
 * you can create entities and add components to them.
 */

/* Forward declaration to the ES::Engine::Entity class
 * Required to avoid include loop between Entity and Core headers
 */
class Entity;

class APlugin;

class Core {
  private:
    using USystem = std::function<void(Core &)>;

  public:
    Core();
    ~Core() = default;

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
    ES::Engine::Entity CreateEntity();

    /**
     * Store a resource instance.
     * Resources are unique struct or class (like a singleton) that contains global informations.
     * Example: AssetsManager, TimeProvider, WindowResource, SceneManager, InputManager or NetworkManager.
     *
     * @tparam  TResource   type of the resource to add
     * @param   resource    rvalue of the resource to add
     * @return  reference of the added resource
     */
    template <typename TResource> TResource &RegisterResource(TResource &&resource);

    /**
     * Get a reference's resource.
     * Resources are unique struct or class (like a singleton) that contains global informations.
     * Example: AssetsManager, TimeProvider, WindowResource, SceneManager, InputManager or NetworkManager.
     *
     * @tparam  TResource   type of the resource to get
     * @return  reference of the resource
     */
    template <typename TResource> TResource &GetResource();

    /**
     * Add a new scheduler to the registry.
     * A scheduler is a class that inherit from IScheduler and that will be called by the registry.
     *
     * @tparam TScheduler The type of scheduler to use.
     * @param scheduler The scheduler to add.
     */
    template <typename TScheduler, typename... Args> TScheduler &RegisterScheduler(Args &&...args);

    /**
     * Get a scheduler from the registry.
     *
     * @tparam TScheduler The type of scheduler to get.
     * @return The scheduler.
     */
    template <typename TScheduler> TScheduler &GetScheduler();

    /**
     * Get the running state of the core
     *
     * @return The running state.
     */
    bool IsRunning();

    /**
     * Stop the core execution
     */
    void Stop();

    /**
     * Execute the core loop
     */
    void RunCore();

    /**
     * Add one or multiple systems to the registry. A system is a function that will be called by the registry.
     * The function must take a Registry as first parameter.
     * The function must return void.
     * The function will be called by the registry according to the scheduler choosen.
     * If multiple systems are added, they will be called as a group, in the order they were added.
     *
     * @tparam  TScheduler  The type of scheduler to use.
     * @param   systems    The systems to add.
     * @see IScheduler
     */
    template <typename TScheduler = ES::Engine::Scheduler::Update, typename... Systems>
    void RegisterSystem(Systems... systems);

    /**
     * Deletes a scheduler from the registry.
     *
     * @tparam TScheduler The type of scheduler to delete.
     * @note This will delete the scheduler at the end of the frame.
     */
    template <typename TScheduler> void DeleteScheduler();

    /**
     * Run all the systems. The systems will be called in the order they were added. It will also update the delta time.
     */
    void RunSystems();

    /**
     * Check if entity is valid in the context of the registry. It check if the id of the entity exist.
     */
    bool IsEntityValid(entt::entity entity);

    /**
     * @brief Adds multiple plugins to the core engine.
     *
     * This function template allows the addition of multiple plugins to the core engine.
     *
     * @tparam TPlugins Variadic template parameter pack representing the types of the plugins to be added.
     */
    template <typename... TPlugins> void AddPlugins();

    /**
     * @brief Checks if a plugin of the specified type is present.
     *
     * @tparam TPlugin The type of the plugin to check for.
     * @return true if the plugin is present, false otherwise.
     */
    template <typename TPlugin> bool HasPlugin() const;

    /**
     * @brief Checks if a plugin of the specified type is present.
     *
     * @warning If you know the type of the plugin at compile time, prefer the template version of this function.
     *
     * @param type The type of the plugin to check for.
     * @return true if the plugin is present, false otherwise.
     */
    bool HasPlugin(std::type_index type) const;

    /**
     * Clear all entities and components from the registry.
     */
    void ClearEntities();

  private:
    /**
     * @brief Adds a plugin of type TPlugin to the engine.
     *
     * This function template allows the addition of a plugin to the engine. The plugin
     * type is specified as a template parameter.
     *
     * @tparam TPlugin The type of the plugin to be added.
     */
    template <typename TPlugin> void AddPlugin();

  private:
    std::unique_ptr<entt::registry> _registry;
    ES::Engine::SchedulerContainer _schedulers;
    std::vector<std::type_index> _schedulersToDelete;
    std::unordered_map<std::type_index, std::unique_ptr<APlugin>> _plugins;
    bool _running = false;
};
} // namespace ES::Engine

#include "Core.inl"
