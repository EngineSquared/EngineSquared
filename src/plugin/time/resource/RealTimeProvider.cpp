#include "RealTimeProvider.hpp"

namespace ES::Plugin::Time::Resource {
RealTimeProvider::RealTimeProvider() { _lastTime = std::chrono::high_resolution_clock::now(); }

float RealTimeProvider::GetElapsedTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration<float>(currentTime - _lastTime).count();

    return elapsedTime;
}

void RealTimeProvider::Update() { _lastTime = std::chrono::high_resolution_clock::now(); }
} // namespace ES::Plugin::Time::Resource
