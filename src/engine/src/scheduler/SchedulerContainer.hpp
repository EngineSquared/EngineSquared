#pragma once

#include "scheduler/AScheduler.hpp"
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Engine {
class Core;

/// @brief Custom exception class for scheduler-related errors.
/// @todo Put this in Exception folder.
class SchedulerError : public std::exception {
  public:
    /// @brief Constructor for SchedulerError.
    /// @param message The error message to be included in the exception.
    explicit SchedulerError(const std::string &message) : msg(fmt::format("Scheduler error: {}", message)) {};

    /// @brief Gets the error message.
    /// @return The error message as a C-style string.
    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    /// @brief The error message associated with the exception.
    std::string msg;
};

/// @class SchedulerContainer
/// @brief Manages a collection of schedulers, allowing addition, retrieval, and deletion of schedulers.
/// @see Engine::Scheduler::IScheduler
/// @see Engine::Scheduler::AScheduler
/// @see Engine::Scheduler::SchedulerError
class SchedulerContainer {
  public:
    /// @brief Default constructor for SchedulerContainer.
    SchedulerContainer() = default;

    /// @brief Destructor for SchedulerContainer.
    ~SchedulerContainer() = default;

    /// @brief Adds a scheduler to the container.
    /// @param core A reference to the Core.
    /// @param args The arguments to be forwarded to the scheduler's constructor.
    /// @tparam TScheduler The type of the scheduler to be added.
    /// @tparam Args The types of the arguments to be forwarded to the scheduler's constructor.
    template <typename TScheduler, typename... Args> void AddScheduler(Core &core, Args &&...args);

    /// @brief Retrieves a scheduler of the specified type.
    /// @tparam TScheduler The type of the scheduler to retrieve.
    template <typename TScheduler> TScheduler &GetScheduler();

    /// @brief Retrieves a scheduler identified by the given type index.
    /// @param id The type index of the scheduler to retrieve.
    /// @return A shared pointer to the scheduler of the specified type.
    std::shared_ptr<Scheduler::AScheduler> GetScheduler(std::type_index id);

    /// @brief Runs all schedulers in the container. This function iterates through the ordered list of schedulers and
    ///   calls the RunSystems method on each scheduler. It ensures that the schedulers are executed in the order
    ///   defined by their dependencies.
    void RunSchedulers();

    /// @brief Deletes a scheduler of the specified type.
    /// @tparam TScheduler The type of the scheduler to be deleted.
    /// @todo Put the implementation in the inl file
    template <typename TScheduler> void DeleteScheduler() { DeleteScheduler(std::type_index(typeid(TScheduler))); }

    /// @brief Deletes a scheduler identified by the given type index.
    /// @param id The type index of the scheduler to be deleted.
    void DeleteScheduler(std::type_index id);

    /// @brief Add a dependency between two schedulers. It will set the first scheduler to be before the second one.
    /// @tparam TBefore The type of the first scheduler.
    /// @tparam TAfter The type of the second scheduler.
    template <typename TBefore, typename TAfter> void Before();

    /// @brief Add a dependency between two schedulers. It will set the first scheduler to be after the second one.
    /// @tparam TAfter The type of the first scheduler.
    /// @tparam TBefore The type of the second scheduler.
    template <typename TAfter, typename TBefore> void After();

    /// @brief Remove a dependency between two schedulers. It will remove the dependency that the first scheduler is
    ///   before the second one.
    /// @tparam TBefore The type of the first scheduler.
    /// @tparam TAfter The type of the second scheduler.
    template <typename TBefore, typename TAfter> void RemoveDependencyBefore();

    /// @brief Remove a dependency between two schedulers. It will remove the dependency that the first scheduler is
    ///   after the second one.
    /// @tparam TAfter The type of the first scheduler.
    /// @tparam TBefore The type of the second scheduler.
    template <typename TAfter, typename TBefore> void RemoveDependencyAfter();

    /// @brief Checks if a scheduler of the specified type exists in the container.
    /// @tparam TScheduler The type of the scheduler to check for.
    /// @return true if the scheduler exists, false otherwise.
    template <typename TScheduler> bool Contains() const;

    /// @brief Checks if a scheduler identified by the given type index exists in the container.
    /// @param id The type index of the scheduler to check for.
    /// @return true if the scheduler exists, false otherwise.
    bool Contains(std::type_index id) const;

    /// @brief Sets the error policy for all schedulers in the container.
    /// @param policy The error policy to be applied to all schedulers.
    /// @see Engine::Scheduler::SchedulerErrorPolicy
    void SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy policy);

  private:
    /// @brief Updates the order of schedulers based on their dependencies. This function performs a topological sort on
    ///   the schedulers to determine the correct execution order. It checks for circular dependencies.
    void Update();

    /// @brief Apply a topological sort between all schedulers based on their dependencies.
    void TopologicalSort();

    /// @brief Helper function to process the dependencies of a scheduler during the topological sort. It updates the
    ///   in-degree of the dependent schedulers and adds them to the queue if their in-degree becomes zero.
    /// @param current The type index of the current scheduler being processed.
    /// @param q The queue used for the topological sort, containing schedulers with zero in-degree.
    /// @param inDegree A map that keeps track of the in-degree of each scheduler, which is the number of dependencies
    ///   it has.
    void ProcessDependencies(std::type_index current, std::queue<std::type_index> &q,
                             std::map<std::type_index, size_t> &inDegree) const;

  private:
    /// @brief A flag indicating whether the order of schedulers needs to be updated.
    bool _dirty = false;

    /// @brief A map of the schedulers.
    std::map<std::type_index, std::shared_ptr<Scheduler::AScheduler>> _schedulers;

    /// @brief A map of the dependencies between schedulers. The key is the type index of a scheduler, and the
    ///   value is a set of type indices of schedulers that depend on the key scheduler.
    std::unordered_map<std::type_index, std::unordered_set<std::type_index>> _dependencies;

    /// @brief A list of the the ordered list of schedulers.
    std::list<std::shared_ptr<Scheduler::AScheduler>> _orderedSchedulers;
};
} // namespace Engine

#include "SchedulerContainer.inl"
