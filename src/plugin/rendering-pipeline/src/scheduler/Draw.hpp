#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Draw : public ES::Engine::Scheduler::AScheduler {
  public:
    using AScheduler::AScheduler;
    void RunSystems() override;

    inline float GetDeltaTime() const { return _elapsedTime; }

  private:
    float _elapsedTime = 0.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();
};
}
