#pragma once

#include <chrono>

namespace ES::Plugin::Time::Resource {
class RealTimeProvider {
  public:
    RealTimeProvider();
    ~RealTimeProvider() = default;

    /**
     * Get the elapsed time since the last time update
     *
     * @return the elapsed time in seconds
     */
    inline float GetElapsedTime() const { return _elapsedTime; };

    /**
     * Update the time provider
     */
    void Update();

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime;
    float _elapsedTime;
};
} // namespace ES::Plugin::Time::Resource
