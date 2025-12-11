# Issue #014: Implement Trigger Events System

**Milestone:** v0.6 - Collision Events & Constraints  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 2-3 days  
**Dependencies:** #013 (Collision Events)  
**Related Issues:** #011 (Overlap Queries)

---

## 📋 Description

Implement trigger/sensor system for overlap detection without physical collision response. Essential for proximity detection, game triggers, pickups, zone activation, and non-physical interactions.

Triggers provide event-driven overlap detection more efficiently than polling OverlapQuery every frame.

## 🎯 Goals

- ✅ Implement **Thread-Safe** Trigger Architecture (Jolt Worker Threads -> Main Thread Dispatch)
- ✅ Implement `Trigger` component (Marks a collider as a sensor)
- ✅ Implement `TriggerListener` component (Opt-in for maximum performance)
- ✅ Implement `TriggerEnterEvent`
- ✅ Implement `TriggerExitEvent`
- ✅ No physical collision (objects pass through)
- ✅ ECS event-driven architecture via `EventManager`
- ✅ Full test coverage & Documentation

## 🔧 Technical Details

### ⚠️ Critical Architecture: Thread Safety & Performance

Similar to Collision Events (#013), Trigger events originate from Jolt's worker threads via the `ContactListener` (when one body is a Sensor).
**Directly accessing the Engine Core, Registry, or EventManager from these callbacks is unsafe.**

**The "Buffered & Filtered" Strategy:**
1.  **Worker Threads (Jolt):**
    *   Detect overlap with a Sensor body.
    *   Buffer the event (Entity IDs, Type: Enter/Exit).
    *   *No ECS access here.*
2.  **Main Thread (PhysicsUpdate):**
    *   Call `ProcessBufferedEvents()`.
    *   **Validation:** Check if entities are still valid.
    *   **Optimization (Filtering):** Check if the sensor entity has the `TriggerListener` component. If NOT, discard the event immediately.
3.  **Dispatch:**
    *   If listener exists, construct `TriggerEnterEvent` / `TriggerExitEvent` and push to `EventManager`.

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Trigger.hpp                 # New file (Component to mark body as sensor)
│   ├── TriggerListener.hpp         # New file (Opt-in for events)
│   └── TriggerEvent.hpp            # New file (Event structs)
├── system/
│   └── TriggerSystem.hpp           # (Optional) Logic can reside in PhysicsUpdate
├── utils/
│   └── ContactListenerImpl.hpp     # Reuse existing listener, handle sensor cases
└── tests/
    └── TriggerSystemTest.cpp       # New file
```

### Trigger Component

```cpp
/**************************************************************************
 * @file Trigger.hpp
 * @brief Trigger/sensor component (overlap without collision)
 **************************************************************************/

#pragma once

#include <cstdint>

namespace Physics::Component {

/**
 * @brief Trigger/sensor component
 *
 * Marks a collider as a trigger - objects pass through without
 * physical collision response, but TriggerEnter/Exit events are fired.
 *
 * **Requirement:** The entity MUST also have a `RigidBody` and a `Collider`.
 * The `RigidBody` should typically be Static or Kinematic.
 */
struct Trigger {
    /// Layer mask (which layers can trigger)
    uint32_t layerMask = 0xFFFFFFFF;  ///< Collide with all layers by default

    static Trigger Default() { return {0xFFFFFFFF}; }
    static Trigger CharacterOnly() { return {1u << CollisionLayers::CHARACTER}; }
};

} // namespace Physics::Component
```

### TriggerEvent Components

```cpp
/**************************************************************************
 * @file TriggerEvent.hpp
 * @brief Trigger event components (enter, exit)
 **************************************************************************/

#pragma once

#include "entity/Entity.hpp"

namespace Physics::Event {

/**
 * @brief Trigger Enter Event
 * Fired when an entity enters a trigger volume.
 */
struct TriggerEnterEvent {
    Engine::Entity triggerEntity;   ///< The entity with the Trigger component
    Engine::Entity otherEntity;     ///< The entity entering the trigger
};

/**
 * @brief Trigger Exit Event
 * Fired when an entity exits a trigger volume.
 */
struct TriggerExitEvent {
    Engine::Entity triggerEntity;   ///< The entity with the Trigger component
    Engine::Entity otherEntity;     ///< The entity exiting the trigger
};

} // namespace Physics::Event
```

### TriggerListener Component (Opt-in)

```cpp
/**************************************************************************
 * @file TriggerListener.hpp
 * @brief Opt-in component for trigger event filtering
 **************************************************************************/

#pragma once

namespace Physics::Component {

/**
 * @brief Trigger listener configuration
 *
 * Add this component to trigger entities that should receive events.
 * **Without this component, trigger events are NOT fired.**
 */
struct TriggerListener {
    bool listenEnter = true;   ///< Fire TriggerEnterEvent
    bool listenExit = true;    ///< Fire TriggerExitEvent

    static TriggerListener Default() { return {true, true}; }
    static TriggerListener EnterOnly() { return {true, false}; }
};

} // namespace Physics::Component
```

## 📝 Implementation Tasks

### Phase 1: Core Architecture
- [ ] Ensure `ContactListenerImpl` handles Sensor contacts correctly.
- [ ] Jolt treats Sensors as contacts, so `OnContactAdded` is called.
- [ ] Check `inBody.IsSensor()` in the callback to distinguish from physical collisions if needed, or handle uniformly and filter later.
- [ ] Buffer these events safely.

### Phase 2: Components & Filtering
- [ ] Create `component/Trigger.hpp`.
- [ ] Create `component/TriggerListener.hpp`.
- [ ] Create `event/TriggerEvent.hpp`.
- [ ] In `ProcessBufferedEvents`:
    - [ ] Check if one of the entities is a Sensor (via `Trigger` component or Jolt body user data).
    - [ ] Check for `TriggerListener`.
    - [ ] Dispatch `TriggerEnterEvent` / `TriggerExitEvent`.

### Phase 3: Integration & Testing
- [ ] Update `RigidBodySystem` or `ColliderFactory` to set `IsSensor = true` on Jolt bodies when `Trigger` component is present.
- [ ] Create `tests/TriggerSystemTest.cpp`.

## 📚 Usage Example

```cpp
// 1. Create a Trigger Entity
auto trigger = core.CreateEntity();
trigger.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());
trigger.AddComponent<Physics::Component::BoxCollider>(glm::vec3(1.0f));
trigger.AddComponent<Physics::Component::Trigger>(); // Mark as sensor
trigger.AddComponent<Physics::Component::TriggerListener>(); // Listen for events

// 2. Subscribe to events
eventManager.RegisterCallback<Physics::Event::TriggerEnterEvent>(
    [](Engine::Core& core, const Physics::Event::TriggerEnterEvent& event) {
        Logger::Info("Entity {} entered trigger {}",
            event.otherEntity, event.triggerEntity);
    }
);
```
