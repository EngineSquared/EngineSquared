#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "FixedTimeUpdate.hpp"
#include "IScheduler.hpp"
#include "RelativeTimeUpdate.hpp"
#include "Startup.hpp"
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
     * Clear all entities and components from the registry.
     */
    void ClearEntities();

  private:
    std::unique_ptr<entt::registry> _registry;

    class SchedulerContainer {
      public:
        SchedulerContainer() = default;
        ~SchedulerContainer() = default;

        template <typename TScheduler, typename... Args> void AddScheduler(ES::Engine::Core &core, Args &&...args)
        {
            if (_idToIndex.contains(std::type_index(typeid(TScheduler))))
            {
                ES::Utils::Log::Warn(fmt::format("Scheduler already exists: {}", typeid(TScheduler).name()));
                return;
            }
#ifdef DEBUG
            ES::Utils::Log::Info(fmt::format("Adding scheduler: {}", typeid(TScheduler).name()));
#endif
            std::size_t index = _orderedSchedulers.size();
            auto schedulerPtr = std::make_unique<TScheduler>(core, std::forward<Args>(args)...);
            _orderedSchedulers.push_back(std::move(schedulerPtr));
            _idToIndex[std::type_index(typeid(TScheduler))] = index;
        }

        template <typename TScheduler> inline TScheduler &GetScheduler()
        {
            return *static_cast<TScheduler *>(
                _orderedSchedulers[_idToIndex[std::type_index(typeid(TScheduler))]].get());
        }

        inline std::vector<std::unique_ptr<Scheduler::AScheduler>> &GetSchedulers() { return _orderedSchedulers; }

        template <typename TScheduler> void DeleteScheduler() { DeleteScheduler(std::type_index(typeid(TScheduler))); }

        void DeleteScheduler(std::type_index id)
        {
            if (_idToIndex.contains(id))
            {
#ifdef DEBUG
                ES::Utils::Log::Info(fmt::format("Deleting scheduler: {}", id.name()));
#endif
                _orderedSchedulers.erase(_orderedSchedulers.begin() + _idToIndex[id]);
                for (auto &[key, value] : _idToIndex)
                {
                    if (value > _idToIndex[id])
                    {
                        value--;
                    }
                }
                _idToIndex.erase(id);
            }
            else
            {
                ES::Utils::Log::Warn(fmt::format("Scheduler does not exist: {}", id.name()));
            }
        }

      private:
        std::unordered_map<std::type_index, std::size_t> _idToIndex; ///< Map to store unique ids for each scheduler.
        // TODO: Have a better data structure to avoid moving all the elements when deleting a scheduler.
        std::vector<std::unique_ptr<Scheduler::AScheduler>>
            _orderedSchedulers; ///< Vector to store schedulers in order.
    };

    SchedulerContainer _schedulers;
    std::vector<std::type_index> _schedulersToDelete;
};
} // namespace ES::Engine

#include "Core.inl"
