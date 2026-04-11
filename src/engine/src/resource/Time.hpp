#pragma once

#include "core/Core.hpp"
#include <chrono>

namespace Engine::Resource {
/// @struct Time
/// @brief Resource that stores the elapsed time since the last frame.
struct Time {
    /// @brief Elapsed time since the last frame in seconds.
    float _elapsedTime = 0.0f;

    /// @brief The time point of the last frame. It is used to calculate the elapsed time since the last frame.
    std::chrono::time_point<std::chrono::high_resolution_clock> _lastTime = std::chrono::high_resolution_clock::now();

    /// @brief Update the _elapsedTime since the last frame. It should be called at the beginning of each frame to
    ///     update the elapsed time. This is a system.
    /// @param core Reference to the core.
    /// @see Engine::Resource::Time::_elapsedTime
    static void Update(Core &core);
};
} // namespace Engine::Resource
