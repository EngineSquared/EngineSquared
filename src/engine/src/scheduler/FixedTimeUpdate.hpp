#pragma once

#include "scheduler/AScheduler.hpp"
#include <chrono>
#include <entt/entt.hpp>

namespace Engine::Scheduler {
/// @class FixedTimeUpdate
/// @brief FixedTimeUpdate is a scheduler that runs systems at a fixed rate
///   It is made to only be run a certain amount of times per second, skipping updates when
///   the framerate is high and running multiple updates when the framerate is low.
///   The time that passes is accumulated if the time between updates is greater than the tick rate
///   or if there is a remainder from the last update(s).
/// @see Engine::Scheduler::AScheduler
class FixedTimeUpdate : public AScheduler {
  private:
    /// @brief The default tick rate for the FixedTimeUpdate scheduler. It is set to 1/50 seconds, which means that the
    ///   scheduler will run at 50 updates per second by default.
    inline static constexpr float DEFAULT_TICK_RATE = 1.0f / 50.0f;

  public:
    /// @brief Constructor of the FixedTimeUpdate scheduler. It takes a reference to the core and an optional tick rate.
    ///   If the tick rate is not provided, it will be set to the default tick rate (DEFAULT_TICK_RATE).
    /// @param core Reference to the core.
    /// @param tickRate The tick rate for the scheduler. It is the time in seconds between each update. It is
    ///   recommended to set this value to a value that is a divisor of the frame time to avoid issues with the
    ///   accumulation of time. For example, if the frame time is 16ms (60 FPS), it is recommended to set the tick rate
    ///   to 1/60 seconds or 1/120 seconds.
    /// @see Engine::Core
    /// @see Engine::Scheduler::FixedTimeUpdate::DEFAULT_TICK_RATE
    FixedTimeUpdate(Core &core, float tickRate = DEFAULT_TICK_RATE);

    /// @copydoc Engine::Scheduler::IScheduler
    void RunSystems() override;

    /// @brief Get the fixed tick rate.
    /// @return The tick rate in seconds
    /// @see Engine::Scheduler::FixedTimeUpdate::_tickRate
    float GetTickRate() const;

    /// @brief Set the fixed tick rate.
    /// @param tickRate The fixed tick rate
    /// @note This can cause issues if the value is changed during an update. It is recommended to change this value
    ///   before the update loop starts.
    /// @see Engine::Scheduler::FixedTimeUpdate::_tickRate
    void SetTickRate(float tickRate);

  private:
    /// @brief The tick rate correspond to the number of update the scheduler should do in a second.
    float _tickRate;
    /// @brief Buffered time since the last update, this allow to not lose time.
    float _bufferedTime = 0.0f;
};
} // namespace Engine::Scheduler
