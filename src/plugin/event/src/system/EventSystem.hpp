#pragma once

#include "core/Core.hpp"
#include "resource/EventManager.hpp"

namespace Event::System {

template <typename TScheduler> inline void ProcessEvents(Engine::Core &core)
{
    auto &eventManager = core.GetResource<Resource::EventManager>();
    eventManager.ProcessEvents<TScheduler>();
}

} // namespace Event::System
