#pragma once

namespace Engine {
class Core;
}

namespace Engine::Scheduler {
/// @brief Enum that defines how the scheduler will handle errors
/// @see Engine::Scheduler::IScheduler
/// @see Engine::Scheduler::AScheduler
enum class SchedulerErrorPolicy {
    /// @brief Fail silently
    Silent,
    /// @brief Let the exception propagate
    Nothing,
    /// @brief Just log the error and resume execution
    LogAndContinue,
    /// @brief Log the error, run the next systems and stop execution of other schedulers
    LogAndFinishScheduler,
    /// @brief Log the error and stop execution
    LogAndStop,
};

/// @brief Interface for the schedulers. A scheduler is responsible for running systems according to a specific policy.
///   Schedulers also manage how logic (system) should be handled.
/// @todo Remove this interface and merge it with the AScheduler class.
class IScheduler {
  public:
    /// @brief Virtual destructor for IScheduler.
    virtual ~IScheduler() = default;

    /// @brief Run the systems according to the scheduler policy.
    virtual void RunSystems(void) = 0;

    /// @brief Get the scheduler policy
    /// @return The scheduler policy
    /// @see Engine::Scheduler::SchedulerErrorPolicy
    virtual SchedulerErrorPolicy GetErrorPolicy() const = 0;

    /// @brief Set the scheduler policy
    /// @param errorPolicy The scheduler policy
    /// @see Engine::Scheduler::SchedulerErrorPolicy
    virtual void SetErrorPolicy(SchedulerErrorPolicy errorPolicy) = 0;
};
} // namespace Engine::Scheduler
