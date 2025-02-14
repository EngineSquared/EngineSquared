#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public AScheduler {
  public:
    explicit Update(Registry &registry) : AScheduler(registry) {}
    void RunSystems(std::vector<USystem> systems) override;

  private:
    float _elapsedTime = 0.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();
};
} // namespace ES::Engine::Scheduler
