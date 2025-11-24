#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>

#include "FunctionContainer.hpp"
#include "core/Core.hpp"


namespace Event::Resource {
class EventManager {
  public:
    using EventTypeID = size_t;
    using EventCallbackID = size_t;

    template <typename TEvent>
    using CallBackContainer = FunctionUtils::FunctionContainer<void, Engine::Core &, const TEvent &>;

    EventManager() = default;
    ~EventManager() = default;

    template <typename TEvent, typename TCallBack> EventCallbackID RegisterCallback(TCallBack &&callback)
    {
        EventTypeID typeID = _GetId<TEvent>();
        if (!_events.contains(typeID))
        {
            _events.try_emplace(typeID, std::make_shared<CallBackContainer<TEvent>>());
        }
        auto callbacks = std::any_cast<std::shared_ptr<CallBackContainer<TEvent>>>(_events[typeID]);
        return callbacks->AddFunction(std::forward<TCallBack>(callback));
    }

    template <typename TEvent> void TriggerEvent(Engine::Core &core, const TEvent &event)
    {
        EventTypeID typeID = _GetId<TEvent>();

        if (!_events.contains(typeID))
        {
            return;
        }
        auto &callbacks = std::any_cast<std::shared_ptr<CallBackContainer<TEvent>> &>(_events[typeID]);
        for (auto &callback : callbacks->GetFunctions())
        {
            callback->Call(core, event);
        }
    }

    template <typename TEvent> void UnregisterCallback(EventCallbackID callbackID)
    {
        EventTypeID typeID = _GetId<TEvent>();
        if (!_events.contains(typeID))
        {
            Log::Warn("EventManager::UnregisterCallback: No callbacks registered for this event type.");
            return;
        }
        auto &callbacks = std::any_cast<std::shared_ptr<CallBackContainer<TEvent>> &>(_events[typeID]);
        if (!callbacks->Contains(callbackID))
        {
            Log::Warn("EventManager::UnregisterCallback: Callback ID not found.");
            return;
        }
        callbacks->DeleteFunction(callbackID);
    }

  private:
    static EventCallbackID _GenerateRandomID()
    {
        static EventCallbackID currentID = 0;
        return currentID++;
    }

    template <typename TEvent> static EventTypeID _GetId(void) { return typeid(TEvent).hash_code(); }

    std::unordered_map<EventTypeID, std::any> _events;
};
} // namespace Event::Resource
