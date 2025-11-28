#pragma once

#include "core/Core.hpp"
#include "resource/EventManager.hpp"

namespace Event::System {

inline void ProcessEvents(Engine::Core &core)
{
    auto &eventManager = core.GetResource<Resource::EventManager>();
    eventManager.ProcessEvents(core);
}

} // namespace Event::System
