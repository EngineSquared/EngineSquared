#pragma once

#include <concepts>
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Id.hpp"
#include "Logger.hpp"
#include "plugin/IPlugin.hpp"
#include "scheduler/SchedulerContainer.hpp"
#include "scheduler/Update.hpp"

namespace Engine {

class Entity;

template <typename TScheduler>
concept CScheduler = std::derived_from<TScheduler, Scheduler::AScheduler>;

template <typename TPlugin>
concept CPlugin = std::derived_from<TPlugin, IPlugin>;

/// @class Core
/// @brief The core is the place where all the data of the engine is stored.
/// It contains the registry (entities), the resources and the schedulers(+systems).
/// It allows to manage entities so to create, delete them.
/// It also allows to manage dependencies between schedulers.
/// It also allows to aggregate plugins to it, and then plugins will be able to add resources and systems to the core.
class Core {
  public:
    /// @brief The type of the registry used by the core. It is an alias for entt::basic_registry with Id as entity
    ///     identifier.
    using Registry = entt::basic_registry<Id>;

    /// @brief Default constructor of the core. It will initialize the registry and the scheduler container.
    ///     Some schedulers will be automatically registered like Startup, Update and Shutdown.
    Core();

    /// @brief Destructor of the core. It will clear the registry and the scheduler container.
    ~Core();

    /// @brief Get the entt::registry that contains all components.
    ///     It should be used to update component through systems.
    /// @return registry that contains all components.
    /// @todo put the implementation in the cpp file, (remove inline)
    inline Registry &GetRegistry() { return *_registry; }

    /// @brief Get the entt::registry that contains all components.
    ///     It should be used to update component through systems.
    /// @return registry that contains all components.
    /// @todo put the implementation in the cpp file, (remove inline)
    inline const Registry &GetRegistry() const { return *_registry; }

    /// @brief Create an entity in the context of the registry.
    /// @return The entity created.
    Entity CreateEntity();

    /// @brief Kill an entity. It will remove all components from the entity.
    /// @param entity The entity to kill.
    void KillEntity(Id entity);

    /// @brief Store a resource instance.
    /// @tparam TResource type of the resource to add
    /// @param resource rvalue of the resource to add
    /// @return reference of the added resource
    template <typename TResource> TResource &RegisterResource(TResource &&resource);

    /// @brief Get a reference of a resource.
    /// @tparam TResource type of the resource to get
    /// @return reference of the resource
    template <typename TResource> TResource &GetResource();

    /// @brief Get a const reference of a resource.
    /// @tparam TResource type of the resource to get
    /// @return const reference of the resource
    template <typename TResource> const TResource &GetResource() const;

    /// @brief Check if a resource is registered in the core.
    /// @tparam TResource type of the resource to check
    /// @return Return true if the resource is registered, false otherwise
    template <typename TResource> bool HasResource() const;

    /// @brief Delete a resource from the core.
    /// @tparam TResource type of the resource to delete
    template <typename TResource> void DeleteResource();

    /// @brief Add a new scheduler to the registry.
    /// @tparam TScheduler The type of scheduler to use.
    /// @param args The scheduler to add.
    template <CScheduler TScheduler, typename... Args> TScheduler &RegisterScheduler(Args &&...args);

    /// @brief Get a scheduler from the registry by its type.
    /// @tparam TScheduler The type of scheduler to get.
    /// @return The scheduler.
    template <CScheduler TScheduler> TScheduler &GetScheduler();

    /// @brief Get a scheduler from the registry by its type index.
    /// @param id The type index of the scheduler to get.
    /// @return The scheduler.
    Scheduler::AScheduler &GetScheduler(std::type_index id);

    /// @brief Sets the execution order of two schedulers, ensuring that TSchedulerA is executed before TSchedulerB.
    /// @tparam TSchedulerA The type of the scheduler that should execute first.
    /// @tparam TSchedulerB The type of the scheduler that should execute after TSchedulerA.
    /// @todo Move the implementation of this function in the .inl file
    template <typename TSchedulerA, typename TSchedulerB> inline void SetSchedulerBefore()
    {
        this->_schedulers.Before<TSchedulerA, TSchedulerB>();
    }

    /// @brief Sets the execution order of two schedulers by specifying that one scheduler should execute after another.
    /// @tparam TSchedulerA The type of the scheduler that should execute first.
    /// @tparam TSchedulerB The type of the scheduler that should execute after TSchedulerA.
    /// @todo Move the implementation of this function in the .inl file
    template <typename TSchedulerA, typename TSchedulerB> inline void SetSchedulerAfter()
    {
        this->_schedulers.After<TSchedulerA, TSchedulerB>();
    }

    /// @brief Removes a dependency between two schedulers, ensuring that TSchedulerB is no longer dependent on
    ///     TSchedulerA.
    /// @tparam TSchedulerA The type of the first scheduler (the one being depended on).
    /// @tparam TSchedulerB The type of the second scheduler (the one depending on TSchedulerA).
    /// @todo Move the implementation of this function in the .inl file
    template <typename TSchedulerA, typename TSchedulerB> inline void RemoveDependencyAfter()
    {
        this->_schedulers.RemoveDependencyAfter<TSchedulerA, TSchedulerB>();
    }

    /// @brief Removes a dependency between two schedulers, ensuring that TSchedulerA is no longer dependent on
    ///     TSchedulerB.
    /// @tparam TSchedulerA The type of the first scheduler (the one depending on TSchedulerB).
    /// @tparam TSchedulerB The type of the second scheduler (the one being depended on).
    /// @todo Move the implementation of this function in the .inl file
    template <typename TSchedulerA, typename TSchedulerB> inline void RemoveDependencyBefore()
    {
        this->_schedulers.RemoveDependencyBefore<TSchedulerA, TSchedulerB>();
    }

    /// @brief Get the running state of the core.
    /// @return The running state.
    bool IsRunning();

    /// @brief Stop the core execution. It will finish the current loop, call shutdown systems if any and stop.
    void Stop();

    /// @brief Execute the core loop, which will run all the schedulers.
    void RunCore();

    /// @brief Add one or multiple systems associated with a specific scheduler.
    /// @tparam TScheduler The type of the scheduler to use. It must be derived from AScheduler. You can look at
    ///     CScheduler
    /// @tparam Systems The types of the systems to add. This is not a mandatory template parameter, you can add only
    ///     the system variable and it will be deduced by the compiler. You can add as many systems as you want, they
    ///     will be called in the order they were added.
    /// @param systems The systems to add.
    /// @return Return the identifiers of the systems added, in the same order as the systems were added.
    /// @see FunctionUtils::FunctionID.
    template <CScheduler TScheduler, typename... Systems> decltype(auto) RegisterSystem(Systems... systems);

    /// @brief Add one or multiple systems associated to the default scheduler.
    /// @tparam Systems The types of the systems to add. This is not a mandatory template parameter, you can add only
    ///     the system variable and it will be deduced by the compiler. You can add as many systems as you want, they
    ///     will be called in the order they were added.
    /// @param systems The systems to add.
    /// @return Return the identifiers of the systems added, in the same order as the systems were added.
    /// @see FunctionUtils::FunctionID.
    template <typename... Systems> decltype(auto) RegisterSystem(Systems... systems);

    /// @brief Add a system to a @b specific scheduler, associated with a callback that should run if the system fails
    ///     (throw).
    /// @tparam TScheduler The type of scheduler to use. It must be derived from AScheduler. See Engine::CScheduler.
    /// @param system The system to add.
    /// @param callback The callback to run if the system fails (throw).
    /// @return Return the identifier of the system added. See FunctionUtils::FunctionID.
    template <CScheduler TScheduler, typename System, typename ErrorCallback>
    decltype(auto) RegisterSystemWithErrorHandler(System system, ErrorCallback callback);

    /// @brief Add a system to the @b default scheduler, associated with a callback that should run if the system fails
    ///     (throw).
    /// @tparam TScheduler The type of scheduler to use.
    /// @see Engine::CScheduler.
    /// @param system The system to add.
    /// @param callback The callback to run if the system fails (throw).
    /// @return Return the identifier of the system added. See FunctionUtils::FunctionID.
    template <typename System, typename ErrorCallback>
    decltype(auto) RegisterSystemWithErrorHandler(System system, ErrorCallback callback);

    /// @brief Deletes a scheduler.
    /// @tparam TScheduler The type of scheduler to delete. See Engine::CScheduler.
    /// @note This function will not delete the scheduler immediately, but it will mark it for deletion. The scheduler
    ///     will be deleted at the end of the current loop, after all systems have been executed.
    template <CScheduler TScheduler> void DeleteScheduler();

    /// @brief Run all the systems once by calling each scheduler.
    void RunSystems();

    /// @brief Check if entity is valid in the context of the registry. It check if the id of the entity exist.
    /// @return true if the entity is valid, false otherwise.
    bool IsEntityValid(Id entity) const;

    /// @brief Adds multiple plugins that will be call instantly through the Bind method to agregate their systems and
    /// resources to the core.
    /// @note 2 same plugins can't be added.
    /// @tparam TPlugins Types of the plugins to add. See Engine::CPlugin.
    template <CPlugin... TPlugins> void AddPlugins();

    /// @brief Checks if a plugin is present.
    /// @tparam TPlugin The type of the plugin to check for. See Engine::CPlugin.
    /// @return true if the plugin is present, false otherwise.
    template <CPlugin TPlugin> bool HasPlugin() const;

    /// @brief Checks if a plugin of the specified type is present.
    /// @param type The type of the plugin to check for.
    /// @return true if the plugin is present, false otherwise.
    bool HasPlugin(std::type_index type) const;

    /// @brief Clear all entities and components from the registry.
    void ClearEntities();

    /// @brief Sets the default scheduler.
    /// @tparam TScheduler The type of the scheduler to be set as default. See Engine::CScheduler.
    /// @todo Move the implementation of this function in the .inl file.
    template <CScheduler TScheduler> inline void SetDefaultScheduler()
    {
        SetDefaultScheduler(std::type_index(typeid(TScheduler)));
    }

    /// @brief Sets the default scheduler.
    /// @param scheduler The type index of the scheduler to be set as default.
    /// @note The scheduler must be present in the core.
    void SetDefaultScheduler(std::type_index scheduler);

    /// @brief Sets the error policy for all schedulers.
    /// @param policy The error policy to set for all schedulers. See Engine::Scheduler::SchedulerErrorPolicy.
    void SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy policy);

  private:
    /// @brief Adds a plugin.
    /// @tparam TPlugin The type of the plugin to add. See Engine::CPlugin.
    template <CPlugin TPlugin> void AddPlugin();

  private:
    /// @brief The registry that contains all components (and by definition, the entities).
    std::unique_ptr<Registry> _registry;
    /// @brief The container that contains all the schedulers.
    Engine::SchedulerContainer _schedulers;
    /// @brief The default scheduler type index. It is used when adding systems without specifying a scheduler.
    std::type_index _defaultScheduler = typeid(Engine::Scheduler::Update);
    /// @brief The list of schedulers to delete at the end of the current loop.
    std::vector<std::type_index> _schedulersToDelete;
    /// @brief The plugins added to the core.
    std::unordered_map<std::type_index, std::unique_ptr<IPlugin>> _plugins;
    /// @brief The running state of the core. It is true if the core is running, false otherwise. It is used to stop the
    ///     core loop.
    bool _running = false;
};
} // namespace Engine

#include "core/Core.ipp"
