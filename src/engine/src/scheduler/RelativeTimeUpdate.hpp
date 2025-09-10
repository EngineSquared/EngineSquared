#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "scheduler/AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief RelativeTimeUpdate is a scheduler that runs systems at a rate that is not fixed
 * It is made to run systems at a rate relative to the time
 */
class RelativeTimeUpdate : public AScheduler {
  private:
    inline static constexpr float DEFAULT_TARGET_TICK_RATE = 1.0f / 50.0f;
    inline static constexpr float REMAINDER_THRESHOLD = 0.0001f;

  public:
    RelativeTimeUpdate(Core &core, float tickRate = DEFAULT_TARGET_TICK_RATE) : AScheduler(core), _tickRate(tickRate) {}

    void RunSystems() override;

    /**
     * @brief Get the target tick rate
     * The target tick rate is the minimum time we want between each system run.
     * If the time between each system run is more than the target tick rate, we will run the systems multiple times
     * Else, we will just run the systems once with the delta time we have
     *
     * @return float The target tick rate
     */
    inline float GetTargetTickRate() const { return _tickRate; }

    /**
     * @brief Set the target tick rate
     *
     * @param tickRate The target tick rate
     * @see GetTargetTickRate
     */
    inline void SetTargetTickRate(float tickRate) { _tickRate = tickRate; }

    /**
     * @brief Get the current delta time
     * The delta time is the time between the last system run and the current system run.
     * This value may be modified by the scheduler to run the systems multiple times
     * if the time between each system run is more than the target tick rate
     *
     * @return float The current delta time
     */
    inline float GetCurrentDeltaTime() const { return _deltaTime; }

  private:
    float _tickRate;
    float _deltaTime = 0.0f;
    float _bufferedTime = 0.0f;
};
} // namespace ES::Engine::Scheduler
