#pragma once

#include <entt/entt.hpp>

#include <chrono>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

class FixedUpdate : public IScheduler {
    inline static constexpr float DEFAULT_TICK_RATE = 1.0 / 50.0;

  public:
    FixedUpdate(Registry &registry, float tickRate = DEFAULT_TICK_RATE)
        : IScheduler(registry), _tickRate(tickRate), _lastTime(std::chrono::high_resolution_clock::now()),
          _elapsedTime(0.0f)
    {
    }
    void operator()(std::vector<USystem> systems)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        _elapsedTime += std::chrono::duration<float>(currentTime - _lastTime).count();
        unsigned int ticks = static_cast<unsigned int>(_elapsedTime / _tickRate);
        _elapsedTime -= ticks * _tickRate;

        for (unsigned int i = 0; i < ticks; i++)
        {
            for (auto &system : systems)
            {
                system(_registry);
            }
        }

        _lastTime = currentTime;
    }

    inline float GetTickRate() const { return _tickRate; }

    inline void SetTickRate(float tickRate) { _tickRate = tickRate; }

  private:
    float _tickRate;
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime;
    float _elapsedTime;
};
} // namespace ES::Engine::Scheduler
