#pragma once

#include <any>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "core/Core.hpp"
#include "scheduler/Update.hpp"
#include "utils/EventContainer.hpp"

namespace Event::Resource {
class EventManager {
  public:
    using EventTypeID = size_t;
    using EventCallbackID = size_t;

    EventManager() = default;
    ~EventManager() = default;

    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    EventManager(EventManager &&other) noexcept : _queueMutex()
    {
        std::scoped_lock lock(other._queueMutex);
        _eventCallbacks = std::move(other._eventCallbacks);
        _eventQueue = std::move(other._eventQueue);
    }

    EventManager &operator=(EventManager &&other) noexcept
    {
        if (this != &other)
        {
            std::scoped_lock lock(other._queueMutex, _queueMutex);
            _eventCallbacks = std::move(other._eventCallbacks);
            _eventQueue = std::move(other._eventQueue);
        }

        return *this;
    }

    template <typename TEvent, typename TCallBack> EventCallbackID RegisterCallback(TCallBack &&callback)
    {
        return _RegisterCallbackImpl<TEvent, TCallBack, Engine::Scheduler::Update>(std::forward<TCallBack>(callback));
    }

    template <typename TEvent, Engine::CScheduler TScheduler, typename TCallBack>
    EventCallbackID RegisterCallback(TCallBack &&callback)
    {
        return _RegisterCallbackImpl<TEvent, TCallBack, TScheduler>(std::forward<TCallBack>(callback));
    }

    template <typename TEvent> void PushEvent(const TEvent &event)
    {
        EventTypeID typeID = _GetId<TEvent>();
        std::scoped_lock lock(_queueMutex);

        for (const auto &[schedulerID, callbacks] : _eventCallbacks)
        {
            if (callbacks.contains(typeID))
            {
                _eventQueue[schedulerID].push({typeID, event});
            }
        }
    }

    template <typename TScheduler> void ProcessEvents(Engine::Core &core)
    {
        auto schedulerID = std::type_index(typeid(TScheduler));

        std::queue<std::pair<EventTypeID, std::any>> queueCopy;
        {
            std::scoped_lock lock(_queueMutex);
            if (_eventQueue.contains(schedulerID))
            {
                std::swap(queueCopy, _eventQueue[schedulerID]);
            }
        }

        while (!queueCopy.empty())
        {
            auto [typeID, event] = std::move(queueCopy.front());
            queueCopy.pop();

            if (_eventCallbacks.contains(schedulerID) && _eventCallbacks[schedulerID].contains(typeID))
            {
                _eventCallbacks[schedulerID][typeID]->Trigger(core, event);
            }
        }
    }

    template <typename TEvent, typename TScheduler = Engine::Scheduler::Update>
    void UnregisterCallback(EventCallbackID callbackID)
    {
        auto schedulerID = std::type_index(typeid(TScheduler));
        EventTypeID typeID = _GetId<TEvent>();
        if (!_eventCallbacks.contains(schedulerID) || !_eventCallbacks[schedulerID].contains(typeID))
        {
            Log::Warn("EventManager::UnregisterCallback: No callbacks registered for this event type.");
            return;
        }
        auto container = std::static_pointer_cast<Utils::EventContainer<TEvent>>(_eventCallbacks[schedulerID][typeID]);
        if (!container->Contains(callbackID))
        {
            Log::Warn("EventManager::UnregisterCallback: Callback ID not found.");
            return;
        }
        container->DeleteFunction(callbackID);
    }

  private:
    template <typename TEvent, typename TCallBack, typename TScheduler>
    EventCallbackID _RegisterCallbackImpl(TCallBack &&callback)
    {
        auto schedulerID = std::type_index(typeid(TScheduler));
        EventTypeID typeID = _GetId<TEvent>();

        if (!_eventCallbacks.contains(schedulerID))
        {
            _eventCallbacks[schedulerID] = {};
        }

        if (!_eventCallbacks[schedulerID].contains(typeID))
        {
            _eventCallbacks[schedulerID].try_emplace(typeID, std::make_shared<Utils::EventContainer<TEvent>>());
        }
        auto container = std::static_pointer_cast<Utils::EventContainer<TEvent>>(_eventCallbacks[schedulerID][typeID]);
        return container->AddFunction(std::forward<TCallBack>(callback));
    }

  private:
    static EventCallbackID _GenerateRandomID()
    {
        static EventCallbackID currentID = 0;
        return currentID++;
    }

    template <typename TEvent> static EventTypeID _GetId(void) { return typeid(TEvent).hash_code(); }

    std::unordered_map<std::type_index, std::unordered_map<EventTypeID, std::shared_ptr<Utils::IEventContainer>>>
        _eventCallbacks;
    std::unordered_map<std::type_index, std::queue<std::pair<EventTypeID, std::any>>> _eventQueue;
    std::mutex _queueMutex;
};
} // namespace Event::Resource
