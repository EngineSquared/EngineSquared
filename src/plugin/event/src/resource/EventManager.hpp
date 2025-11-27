#pragma once

#include <any>
#include <functional>
#include <memory>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "core/Core.hpp"
#include "utils/EventContainer.hpp"

namespace Event::Resource {
class EventManager {
  public:
    using EventTypeID = size_t;
    using EventCallbackID = size_t;

    EventManager() = default;
    ~EventManager() = default;

    template <typename TEvent, typename TCallBack> EventCallbackID RegisterCallback(TCallBack &&callback)
    {
        EventTypeID typeID = _GetId<TEvent>();
        if (!_eventCallbacks.contains(typeID))
        {
            _eventCallbacks.try_emplace(typeID, std::make_shared<Utils::EventContainer<TEvent>>());
        }
        auto container = std::static_pointer_cast<Utils::EventContainer<TEvent>>(_eventCallbacks[typeID]);
        return container->AddFunction(std::forward<TCallBack>(callback));
    }

    template <typename TEvent> void PushEvent(const TEvent &event)
    {
        EventTypeID typeID = _GetId<TEvent>();
        _eventQueue.push({typeID, event});
    }

    void ProcessEvents(Engine::Core &core)
    {
        while (!_eventQueue.empty())
        {
            auto [typeID, event] = std::move(_eventQueue.front());
            _eventQueue.pop();

            if (_eventCallbacks.contains(typeID))
            {
                _eventCallbacks[typeID]->Trigger(core, event);
            }
        }
    }

    template <typename TEvent> void UnregisterCallback(EventCallbackID callbackID)
    {
        EventTypeID typeID = _GetId<TEvent>();
        if (!_eventCallbacks.contains(typeID))
        {
            Log::Warn("EventManager::UnregisterCallback: No callbacks registered for this event type.");
            return;
        }
        auto container = std::static_pointer_cast<Utils::EventContainer<TEvent>>(_eventCallbacks[typeID]);
        if (!container->Contains(callbackID))
        {
            Log::Warn("EventManager::UnregisterCallback: Callback ID not found.");
            return;
        }
        container->DeleteFunction(callbackID);
    }

  private:
    static EventCallbackID _GenerateRandomID()
    {
        static EventCallbackID currentID = 0;
        return currentID++;
    }

    template <typename TEvent> static EventTypeID _GetId(void) { return typeid(TEvent).hash_code(); }

    std::unordered_map<EventTypeID, std::shared_ptr<Utils::IEventContainer>> _eventCallbacks;
    std::queue<std::pair<EventTypeID, std::any>> _eventQueue;
};
} // namespace Event::Resource
