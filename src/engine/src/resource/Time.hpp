#pragma once

#include <chrono>
#include "Core.hpp"

namespace ES::Engine::Resource
{
    struct Time {
        float _elapsedTime = 0.0f;
        std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();

        static void Update(Core &core)
        {
            auto &time = core.GetResource<Time>();
            auto now = std::chrono::high_resolution_clock::now();
            time._elapsedTime = std::chrono::duration<float>(now - time._lastTime).count();
            time._lastTime = now;
        }
    };
}

