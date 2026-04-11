#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/// @class RelativeTimeUpdate
/// @brief RelativeTimeUpdate is a scheduler that runs systems at a defined rate. The tick rate is the maximum
///   time we want between each system run. If the time between each system run is more than the target tick rate, we
///   will run the systems multiple times with a delta time equal to the tick rate. Else, we will just run the systems
///   once with the delta time we have (remaining).
class RelativeTimeUpdate : public AScheduler {
  private:
    /// @brief The default target tick rate for the RelativeTimeUpdate scheduler. It is set to 1/50 seconds, which means
    ///   that the scheduler will try to run the systems at a maximum rate of 50 times per second.
    inline static constexpr float DEFAULT_TARGET_TICK_RATE = 1.0f / 50.0f;
    /// @brief The threshold for the remainder time. If the remainder time is less than this threshold, we will not run
    ///   the systems with the remainder time as delta time, and we will just add the remainder time to the buffered
    ///   time for the next tick. This is to avoid running the systems with a very small delta time, which can cause
    ///   issues.
    inline static constexpr float REMAINDER_THRESHOLD = 0.0001f;

  public:
    /// @brief Constructor of the RelativeTimeUpdate scheduler. It takes a reference to the core and an optional tick
    ///   rate.
    /// @param core Reference to the core.
    /// @param tickRate The target tick rate for the scheduler. If no tick rate is provided, the default target tick
    ///   rate will be used (DEFAULT_TARGET_TICK_RATE).
    /// @see Engine::Core
    /// @see Engine::Scheduler::RelativeTimeUpdate::DEFAULT_TARGET_TICK_RATE
    /// @see Engine::Scheduler::RelativeTimeUpdate::_tickRate
    /// @see Engine::Scheduler::RelativeTimeUpdate::AScheduler
    RelativeTimeUpdate(Core &core, float tickRate = DEFAULT_TARGET_TICK_RATE);

    /// @copydoc AScheduler::RunSystems
    void RunSystems() override;

    /// @brief Get the target tick rate
    /// @return The target tick rate
    /// @see Engine::Scheduler::RelativeTimeUpdate::DEFAULT_TARGET_TICK_RATE
    /// @see Engine::Scheduler::RelativeTimeUpdate::_tickRate
    float GetTargetTickRate() const;

    /// @brief Set the target tick rate
    /// @param tickRate The target tick rate
    /// @see Engine::Scheduler::RelativeTimeUpdate::DEFAULT_TARGET_TICK_RATE
    /// @see Engine::Scheduler::RelativeTimeUpdate::_tickRate
    void SetTargetTickRate(float tickRate);

    /// @brief Get the current delta time
    /// @return The current delta time
    /// @see Engine::Scheduler::RelativeTimeUpdate::_deltaTime
    float GetCurrentDeltaTime() const;

  private:
    /// @brief The target tick rate for the scheduler. It is the maximum time we want between each system run. If the
    ///   time between each system run is more than the target tick rate, we will run the systems multiple times with a
    ///   delta time equal to the tick rate. After tick rate time has passed, if there is a remainder time that is
    ///   greater than the remainder threshold, he will run the systems one more time with the remainder time as delta
    ///   time. Else, he will just add the remainder time to the buffered time for the next tick. This is to avoid
    ///   running the systems with a very small delta time.
    /// @see Engine::Scheduler::RelativeTimeUpdate::DEFAULT_TARGET_TICK_RATE
    /// @see Engine::Scheduler::RelativeTimeUpdate::GetTargetTickRate
    /// @see Engine::Scheduler::RelativeTimeUpdate::SetTargetTickRate
    float _tickRate;

    /// @brief The current delta time. It is the time between the last system run and the current system run. This value
    ///   may be modified by the scheduler to run the systems multiple times if the time between each system run is more
    ///   than the target tick rate.
    /// @see Engine::Scheduler::RelativeTimeUpdate::GetCurrentDeltaTime
    float _deltaTime = 0.0f;

    /// @brief Time accumulated from the previous ticks that is less than the threshold. This time will be added to the
    ///   delta time of the next tick. This is to avoid running the systems with a very small delta time.
    /// @see Engine::Scheduler::RelativeTimeUpdate::REMAINDER_THRESHOLD
    float _bufferedTime = 0.0f;
};
} // namespace Engine::Scheduler
