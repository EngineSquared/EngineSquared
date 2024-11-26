#pragma once
#include <chrono>

namespace ES::Utils {
/**
 * Clock class is used to provide a time provider. It can be used to update the time and get the elapsed tick since the
 * last update. Btw, if you set tick rate to 1, you will get the elapsed time in seconds like a classic time provider.
 */
class Clock {
  public:
    /**
     * Default tick rate of the clock (50 ticks per second)
     */
    inline static const float DEFAULT_TICK_RATE = 1.0 / 50.0;

  public:
    /**
     * Construct a new Clock object
     * If no tick rate is provided, the default tick rate will be used (50 ticks per second)
     */
    Clock(float tickRate = DEFAULT_TICK_RATE)
        : _tickRate(tickRate), _lastTime(std::chrono::high_resolution_clock::now()), _elapsedTime(0.0f),
          _elapsedTicks(0)
    {
    }

    /**
     * Get the elapsed time since the last time update
     *
     * @return the elapsed time in seconds
     */
    unsigned int GetElapsedTicks();

    /**
     * Update the time provider. It will update the elapsed time since the last update and convert it to elapsed ticks
     */
    void Update();

    /**
     * Set the tick rate of the clock
     *
     * @param tickRate the tick rate in seconds
     */
    void SetTickRate(float tickRate) { _tickRate = tickRate; }

    /**
     * Get the tick rate of the clock
     *
     * @return the tick rate in seconds
     */
    float GetTickRate() const { return _tickRate; }

  private:
    float _tickRate;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime;
    float _elapsedTime;
    unsigned int _elapsedTicks;
};

} // namespace ES::Utils
