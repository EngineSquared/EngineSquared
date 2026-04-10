#include "resource/Time.hpp"

namespace Engine::Resource {
void Time::Update(Core &core)
{
    auto &time = core.GetResource<Time>();
    auto now = std::chrono::high_resolution_clock::now();
    time._elapsedTime = std::chrono::duration<float>(now - time._lastTime).count();
    time._lastTime = now;
}
} // namespace Engine::Resource
