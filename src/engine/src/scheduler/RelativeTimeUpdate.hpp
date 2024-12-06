#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

/**
 * @brief RelativeTimeUpdate is a scheduler that runs systems at a rate that is not fixed
 * It is made to run systems at a rate relative to the time
 */
class RelativeTimeUpdate : public IScheduler {
    inline static constexpr float DEFAULT_TARGET_TICK_RATE = 1.0 / 50.0;
    inline static constexpr float REMAINDER_THRESHOLD = 0.0001f;

  public:
    RelativeTimeUpdate(Registry &registry, float tickRate = DEFAULT_TARGET_TICK_RATE)
        : IScheduler(registry), _tickRate(tickRate), _deltaTime(0.0f),
          _lastTime(std::chrono::high_resolution_clock::now())
    {
    }

    void RunSystems(std::vector<USystem> systems) override;

    inline float GetTargetTickRate() const { return _tickRate; }

    inline void SetTargetTickRate(float tickRate) { _tickRate = tickRate; }

    inline float GetCurrentDeltaTime() const { return _deltaTime; }

  private:
    float _tickRate;
    float _deltaTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime;
};
} // namespace ES::Engine::Scheduler
