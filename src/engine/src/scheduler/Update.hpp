#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
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
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();
};
} // namespace ES::Engine::Scheduler
