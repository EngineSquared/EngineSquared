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
/**
 * @brief Thread-safe event manager for registering, queuing, and dispatching events.
 *
 * Manages event callbacks and event queues per scheduler type. Events are queued when pushed
 * and processed during the corresponding scheduler execution. All operations are thread-safe.
 */
class EventManager {
  private:
    struct DirectCallbackSchedulerTag {};

  public:
    /**
     * @brief Type identifier for event types.
     */
    using EventTypeID = size_t;

    /**
     * @brief Unique identifier for registered event callbacks.
     */
    using EventCallbackID = size_t;

    /**
     * @brief Default constructor.
     */
    EventManager() = default;

    /**
     * @brief Default destructor.
     */
    ~EventManager() = default;

    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    /**
     * @brief Move constructor.
     * @param other The EventManager to move from.
     */
    EventManager(EventManager &&other) noexcept : _queueMutex(), _callbacksMutex(), _directCallbackMutex()
    {
        std::scoped_lock lock(other._queueMutex, other._callbacksMutex, other._directCallbackMutex);
        _eventCallbacks = std::move(other._eventCallbacks);
        _eventQueue = std::move(other._eventQueue);
    }

    /**
     * @brief Move assignment operator.
     * @param other The EventManager to move from.
     * @return Reference to this EventManager.
     */
    EventManager &operator=(EventManager &&other) noexcept
    {
        if (this != &other)
        {
            std::scoped_lock lock(other._queueMutex, _queueMutex, other._callbacksMutex, _callbacksMutex,
                                  other._directCallbackMutex, _directCallbackMutex);
            _eventCallbacks = std::move(other._eventCallbacks);
            _eventQueue = std::move(other._eventQueue);
        }

        return *this;
    }

    /**
     * @brief Register a callback for an event type on a specific scheduler.
     * @tparam TEvent The event type to listen for.
     * @tparam TScheduler The scheduler type on which to process this event.
     * @tparam TCallBack The callback type (auto-deduced).
+    * @param callback The callback function with signature void(const TEvent&).
     * @return Unique identifier for the registered callback.
     */
    template <typename TEvent, typename TScheduler = DirectCallbackSchedulerTag, typename TCallBack>
    EventCallbackID RegisterCallback(TCallBack &&callback)
    {
        return _RegisterCallbackImpl<TEvent, TCallBack, TScheduler>(std::forward<TCallBack>(callback));
    }

    /**
     * @brief Queue an event for processing.
     *
     * The event is added to the queue for each scheduler that has registered callbacks
     * for this event type. Events are processed during the corresponding scheduler execution.
     *
     * @tparam TEvent The event type.
     * @param event The event instance to queue.
     */
    template <typename TEvent> void PushEvent(const TEvent &event)
    {
        EventTypeID typeID = _GetId<TEvent>();
        std::scoped_lock lock(_queueMutex, _callbacksMutex);

        for (const auto &[schedulerID, callbacks] : _eventCallbacks)
        {
            if (callbacks.contains(typeID))
            {
                _eventQueue[schedulerID].push({typeID, event});
            }
        }

        auto &directEventCallbacks = _eventCallbacks[std::type_index(typeid(DirectCallbackSchedulerTag))];
        for (const auto &[eventId, _] : directEventCallbacks)
        {
            if (eventId == typeID)
            {
                auto container = std::static_pointer_cast<Utils::EventContainer<TEvent>>(directEventCallbacks[eventId]);
                for (auto &callback : container->GetFunctions())
                {
                    callback->Call(event);
                }
            }
        }
    }

    /**
     * @brief Process all queued events for a specific scheduler.
     *
     * Dequeues and triggers all callbacks registered for the given scheduler type.
     * This method is typically called by the scheduler during its execution phase.
     *
     * @tparam TScheduler The scheduler type whose events should be processed.
     * @param core Reference to the engine core.
     */
    template <typename TScheduler> void ProcessEvents(void)
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

            std::shared_ptr<Utils::IEventContainer> callback;
            {
                std::scoped_lock lock(_callbacksMutex);
                if (_eventCallbacks.contains(schedulerID) && _eventCallbacks[schedulerID].contains(typeID))
                {
                    callback = _eventCallbacks[schedulerID][typeID];
                }
            }

            if (callback)
            {
                callback->Trigger(event);
            }
        }
    }

    /**
     * @brief Unregister a previously registered callback.
     *
     * Removes the callback identified by the given ID for the specified event type
     * and scheduler. Logs a warning if the callback or event type is not found.
     *
     * @tparam TEvent The event type the callback was registered for.
     * @tparam TScheduler The scheduler type (defaults to Update).
     * @param callbackID The unique identifier returned by RegisterCallback.
     */
    template <typename TEvent, typename TScheduler = DirectCallbackSchedulerTag>
    void UnregisterCallback(EventCallbackID callbackID)
    {
        EventTypeID typeID = _GetId<TEvent>();
        std::scoped_lock lock(_callbacksMutex);
        auto schedulerID = std::type_index(typeid(TScheduler));
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
        EventTypeID typeID = _GetId<TEvent>();
        std::scoped_lock lock(_callbacksMutex);
        auto schedulerID = std::type_index(typeid(TScheduler));

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
    std::mutex _callbacksMutex;
    std::mutex _directCallbackMutex;
};
} // namespace Event::Resource
