#pragma once

#include "System.hpp"
#include <entt/entt.hpp>

namespace ES::Engine {
class Core;
}

namespace ES::Engine::Scheduler {
/**
 * @brief Enum that defines how the scheduler will handle errors
 */
enum class SchedulerErrorPolicy {
  /// Fail silently
  Silent,
  /// Just log the error and resume execution
  LogAndContinue,
  /// Log the error, run the next systems and stop execution of other schedulers
  LogAndFinishScheduler,
  /// Log the error and stop execution
  LogAndStop,
};


/**
 * @brief Interface to be implemented for every schedulers
 */
class IScheduler {
  public:
    virtual ~IScheduler() = default;
    /**
     * @brief Run the systems according to the scheduler policy
     *
     * @param systems The systems to run
     */
    virtual void RunSystems(void) = 0;

    /**
     * @brief Get the scheduler policy
     *
     * @return The scheduler policy
     */
    virtual SchedulerErrorPolicy GetErrorPolicy() const = 0;

    /**
     * @brief Set the scheduler policy
     *
     * @param errorPolicy The scheduler policy
     */
    virtual void SetErrorPolicy(SchedulerErrorPolicy errorPolicy) = 0;
};
} // namespace ES::Engine::Scheduler
