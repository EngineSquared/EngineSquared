#include "RealTimeUpdater.hpp"

namespace ES::Plugin::Time::System {
void RealTimeUpdater(ES::Engine::Registry &registry)
{
    registry.GetResource<ES::Plugin::Time::Resource::RealTimeProvider>().Update();
}
} // namespace ES::Plugin::Time::System
