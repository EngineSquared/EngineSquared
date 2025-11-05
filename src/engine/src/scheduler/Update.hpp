#pragma once

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;

    /**
     * @brief Get the current delta time
     * The delta time is the time between the last system run and the current system run.
     *
     * @return float The current delta time
     */
    inline float GetDeltaTime() const { return _elapsedTime; }

  private:
    float _elapsedTime = 0.0f;
};
} // namespace Engine::Scheduler
