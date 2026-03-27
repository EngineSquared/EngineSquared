#pragma once

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/// @class Startup
/// @brief Startup scheduler that runs systems only once.
class Startup : public AScheduler {
  public:
    /// @brief Constructor for the Startup scheduler.
    /// @param core A reference to the Core.
    /// @param callback A callback function that will be called after all systems have been run.
    /// @see Engine::Scheduler::Startup::_callback
    explicit Startup(Core &core, const std::function<void()> &callback) : AScheduler(core), _callback(callback) {}

    /// @copydoc Engine::Scheduler::IScheduler::RunSystems
    void RunSystems() override;

  private:
    /// @brief A callback function that will be called after all systems have been run.
    std::function<void()> _callback;
};
} // namespace Engine::Scheduler
