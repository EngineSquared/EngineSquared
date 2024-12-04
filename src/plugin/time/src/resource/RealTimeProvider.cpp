#include "RealTimeProvider.hpp"

namespace ES::Plugin::Time::Resource {
RealTimeProvider::RealTimeProvider()
{
    _lastTime = std::chrono::high_resolution_clock::now();
    _elapsedTime = 0.f;
}

void RealTimeProvider::Update()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    _elapsedTime = std::chrono::duration<float>(currentTime - _lastTime).count();
    _lastTime = currentTime;
}
} // namespace ES::Plugin::Time::Resource
