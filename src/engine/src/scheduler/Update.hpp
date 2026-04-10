#pragma once

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/// @class Update
/// @brief Update scheduler that runs systems every time it is called
class Update : public AScheduler {
  public:
    /// @copydoc Engine::Scheduler::AScheduler::AScheduler
    using AScheduler::AScheduler;

    /// @copydoc Engine::Scheduler::IScheduler::RunSystems
    void RunSystems() override;

    /// @brief Get the current delta time
    /// @return The current delta time
    float GetDeltaTime() const;

  private:
    /// @brief The elapsed time since the last call to RunSystems.
    float _elapsedTime = 0.0f;
};
} // namespace Engine::Scheduler
