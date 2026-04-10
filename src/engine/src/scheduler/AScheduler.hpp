#pragma once

#include "IScheduler.hpp"

#include "system/System.hpp"

#include <set>
#include <list>
#include <typeindex>

namespace Engine::Scheduler {
/// @brief AScheduler is an abstract class that implements the IScheduler interface. It provides common functionalities
///   for schedulers, such as enabling/disabling systems and error handling. It also provides a default implementation
///   for the RunSystem function, which executes a system according to the scheduler policy.
/// @see Engine::Scheduler::IScheduler
class AScheduler : public IScheduler {
  public:
    /// @brief Constructor of the AScheduler class.
    /// @param core Reference to the core.
    /// @see Engine::Core
    explicit AScheduler(Core &core);

    /// @brief Get systems inside the scheduler.
    /// @return A SystemContainer with enabled systems.
    /// @see Engine::SystemContainer
    const std::list<std::unique_ptr<SystemBase>> &GetSystems();

    /// @brief Add systems to the scheduler.
    /// @tparam TSystems Types of the systems to add.
    /// @param systems The systems to add.
    /// @return A tuple of FunctionIDs for the added systems. See FunctionUtils::FunctionContainer::AddFunctions for
    /// more details.
    template <typename... TSystems> decltype(auto) AddSystems(TSystems... systems);

    /// @brief Disable a system.
    /// @param id The system to disable
    void Disable(FunctionUtils::FunctionID id);

    /// @brief Enable a system.
    /// @param id The system to enable
    void Enable(FunctionUtils::FunctionID id);

    /// @brief Execute a system.
    /// @note The system will be executed according to the scheduler policy of the scheduler.
    /// @param system The system to execute.
    /// @param core The core to pass to the system.
    /// instead.
    /// @see Engine::SystemBase
    void RunSystem(const SystemBase *system, Core &core);

    /// @brief Get whether the next scheduler should run or not. This is mainly set by the error policy of the
    ///   scheduler.
    /// @return true if the next scheduler should run, false otherwise.
    bool ShouldRunNextScheduler() const;

    /// @brief Get the error policy of the scheduler.
    /// @return The error policy of the scheduler.
    /// @see Engine::Scheduler::SchedulerErrorPolicy
    SchedulerErrorPolicy GetErrorPolicy() const override;

    /// @brief Set the error policy of the scheduler.
    /// @param errorPolicy The error policy to set.
    /// @see Engine::Scheduler::SchedulerErrorPolicy
    void SetErrorPolicy(SchedulerErrorPolicy errorPolicy) override;

    /// @brief Remove a system from the scheduler.
    /// @param id The system to remove.
    /// @see FunctionUtils::FunctionID
    void Remove(FunctionUtils::FunctionID id);

  protected:
    /// @brief Reference to the core.
    /// @note This reference is used to pass the core to the systems when executing them. It can also be used by the
    ///   schedulers to access the core and its resources.
    /// @see Engine::Core
    Core &_core;

  private:
    /// @brief List of enabled systems in the scheduler.
    SystemContainer _enabledSystemsList;
    /// @brief List of disabled systems in the scheduler.
    SystemContainer _disabledSystemsList;
    /// @brief A state if the systems of the scheduler should be executed or not. If false, the scheduler will skip the
    ///   execution of its systems. This is mainly used to handle the error policy of the scheduler.
    bool _shouldRunSystems = true;
    /// @brief A state if the next scheduler should be executed or not. This is mainly used to handle the error policy
    ///   of the scheduler.
    bool _shouldRunNextScheduler = true;
    /// @brief The error policy of the scheduler. It defines how the scheduler should handle errors that occur during
    ///   the execution of its systems.
    SchedulerErrorPolicy _errorPolicy = SchedulerErrorPolicy::LogAndContinue;
};
} // namespace Engine::Scheduler

#include "scheduler/AScheduler.ipp"
