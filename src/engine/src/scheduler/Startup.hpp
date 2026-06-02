#pragma once

#include "scheduler/AScheduler.hpp"
#include "scheduler/SchedulerCategory.hpp"

namespace Engine::Scheduler {
/// @class Startup
/// @brief Startup scheduler that runs systems only once.
class Startup : public AScheduler {
  public:
    inline static const SchedulerCategory Category = SchedulerCategory::Startup;
    /// @brief Constructor for the Startup scheduler.
    /// @param core A reference to the Core.
    /// @param callback A callback function that will be called after all systems have been run.
    /// @see Engine::Scheduler::Startup::_callback
    explicit Startup(Core &core);

    /// @copydoc Engine::Scheduler::IScheduler::RunSystems
    void RunSystems() override;
};
} // namespace Engine::Scheduler
