#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief FixedTimeUpdate is a scheduler that runs systems at a fixed rate
 * It is made to only be run a certain amount of times per second, skipping updates when
 * the framerate is high and running multiple updates when the framerate is low.
 * The time that passes is accumulated if the time between updates is greater than the tick rate
 * or if there is a remainder from the last update(s).
 */
class FixedTimeUpdate : public AScheduler {
  private:
    inline static constexpr float DEFAULT_TICK_RATE = 1.0 / 50.0;

  public:
    FixedTimeUpdate(Core &registry, float tickRate = DEFAULT_TICK_RATE) : AScheduler(registry), _tickRate(tickRate) {}
    void RunSystems(std::vector<std::unique_ptr<SystemBase>> &systems) override;

    /**
     * @brief Get the fixed tick rate
     *
     * @return float The fixed tick rate
     */
    inline float GetTickRate() const { return _tickRate; }

    /**
     * @brief Set the fixed tick rate
     *
     * @param tickRate The fixed tick rate
     * @note This can cause issues if the value is changed during an update.
     *      It is recommended to change this value before the update loop starts.
     */
    inline void SetTickRate(float tickRate) { _tickRate = tickRate; }

  private:
    float _tickRate;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();
    float _elapsedTime = 0.0f;
};
} // namespace ES::Engine::Scheduler
