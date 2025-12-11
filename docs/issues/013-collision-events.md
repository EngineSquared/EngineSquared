# Issue #013: Implement Collision Events System

**Milestone:** v0.6 - Collision Events & Constraints  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 3-4 days  
**Dependencies:** #001-012 (Core physics)  
**Related Issues:** #014 (Trigger Events)

---

## 📋 Description

Implement collision event system for detecting and responding to collisions between physics bodies. Essential for gameplay logic, damage systems, sound effects, particle spawning, and game state changes.

Events provide a clean, decoupled way to handle collision responses without polling or manual checks.

## 🎯 Goals

- ✅ Implement **Thread-Safe** Event Architecture (Jolt Worker Threads -> Main Thread Dispatch)
- ✅ Implement `CollisionListener` component (Opt-in for maximum performance)
- ✅ Implement `CollisionAddedEvent` (Enter) with rich contact data
- ✅ Implement `CollisionPersistedEvent` (Stay) - *Use with caution*
- ✅ Implement `CollisionRemovedEvent` (Exit)
- ✅ ECS event-driven architecture via `EventManager`
- ✅ Full test coverage & Documentation

## 🔧 Technical Details

### ⚠️ Critical Architecture: Thread Safety & Performance

Jolt Physics executes simulation steps on **worker threads**. Contact callbacks (`OnContactAdded`, etc.) are invoked concurrently.
**Directly accessing the Engine Core, Registry, or EventManager from these callbacks is unsafe and will cause crashes.**

**The "Buffered & Filtered" Strategy:**
1.  **Worker Threads (Jolt):**
    *   Collect raw contact data (Entity IDs, Position, Normal, Impulse).
    *   Store in thread-safe buffers (e.g., `std::vector` protected by mutex).
    *   *No ECS access here.*
2.  **Main Thread (PhysicsUpdate):**
    *   Call `ProcessBufferedEvents()`.
    *   **Validation:** Check if entities are still valid.
    *   **Optimization (Filtering):** Check if entities have the `CollisionListener` component. If NOT, discard the event immediately.
3.  **Dispatch:**
    *   If listener exists, construct the full `CollisionEvent` and push to `EventManager`.

This ensures **Safety** (no race conditions) and **Performance** (events are only generated for entities that care).

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── CollisionListener.hpp       # New file (opt-in component)
│   └── CollisionEvent.hpp          # New file (Event structs)
├── system/
│   └── CollisionEventSystem.hpp    # (Optional) Logic can reside in PhysicsUpdate or dedicated system
├── utils/
│   ├── ContactListenerImpl.hpp     # Update to support buffering & rich data
│   └── ContactListenerImpl.cpp     # Implementation of buffering
└── tests/
    └── CollisionEventTest.cpp      # New file
```

### CollisionEvent Components

```cpp
/**************************************************************************
 * @file CollisionEvent.hpp
 * @brief Collision event components (enter, stay, exit)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "entity/Entity.hpp"

namespace Physics::Event {

/**
 * @brief Contact point information
 * Describes a single point of contact between two bodies.
 */
struct ContactPoint {
    glm::vec3 position;         ///< Contact point in world space
    glm::vec3 normal;           ///< Contact normal (from A to B)
    float penetrationDepth;     ///< Penetration depth
    float impulse;              ///< Impulse magnitude applied
};

/**
 * @brief Collision Added Event (Enter)
 * Fired when two bodies start touching.
 */
struct CollisionAddedEvent {
    Engine::Entity entityA;
    Engine::Entity entityB;
    ContactPoint contact;       ///< Primary contact point
    // Note: We can store multiple contacts if needed, but usually one is enough for gameplay logic

    /**
     * @brief Get the entity that is NOT 'self'
     */
    Engine::Entity GetOther(Engine::Entity self) const {
        return (self == entityA) ? entityB : entityA;
    }
};

/**
 * @brief Collision Persisted Event (Stay)
 * Fired every physics frame while bodies remain in contact.
 * @warning High performance cost! Only enable if strictly necessary.
 */
struct CollisionPersistedEvent {
    Engine::Entity entityA;
    Engine::Entity entityB;
    ContactPoint contact;
};

/**
 * @brief Collision Removed Event (Exit)
 * Fired once when two bodies stop touching.
 */
struct CollisionRemovedEvent {
    Engine::Entity entityA;
    Engine::Entity entityB;
};

} // namespace Physics::Event
```

### CollisionListener Component (Opt-in)

```cpp
/**************************************************************************
 * @file CollisionListener.hpp
 * @brief Opt-in component for collision event filtering
 **************************************************************************/

#pragma once

namespace Physics::Component {

/**
 * @brief Collision listener configuration
 *
 * Add this component to entities that should receive collision events.
 * **Without this component, collision events are NOT fired for this entity.**
 * This is a critical optimization to avoid flooding the event system.
 */
struct CollisionListener {
    bool listenEnter = true;   ///< Fire CollisionAddedEvent
    bool listenStay = false;   ///< Fire CollisionPersistedEvent (Expensive!)
    bool listenExit = true;    ///< Fire CollisionRemovedEvent

    float minImpulse = 0.0f;   ///< Minimum impulse to trigger event (ignore soft touches)

    static CollisionListener Default() { return {true, false, true, 0.0f}; }
    static CollisionListener ImpactsOnly(float threshold = 1.0f) { return {true, false, false, threshold}; }
};

} // namespace Physics::Component
```

## 📝 Implementation Tasks

### Phase 1: Core Architecture (Thread Safety)
- [ ] Update `ContactListenerImpl` to extract `ContactPoint` data (position, normal, impulse) from Jolt.
- [ ] Ensure `ContactListenerImpl` buffers this data into a thread-safe queue (mutex protected).
- [ ] Implement `ProcessBufferedEvents(Core& core)` to flush the queue on the main thread.

### Phase 2: Components & Filtering
- [ ] Create `component/CollisionListener.hpp`.
- [ ] Create `event/CollisionEvent.hpp` with rich data structs.
- [ ] In `ProcessBufferedEvents`:
    - [ ] Iterate buffered raw contacts.
    - [ ] Check if `entityA` OR `entityB` has `CollisionListener`.
    - [ ] Apply filtering (minImpulse, listenStay flags).
    - [ ] If passed, construct `CollisionAddedEvent` / `Persisted` / `Removed`.
    - [ ] Push to `EventManager`.

### Phase 3: Integration & Testing
- [ ] Register `CollisionListener` component in `PhysicsPlugin`.
- [ ] Create `tests/CollisionEventTest.cpp`:
    - [ ] Test: No listener = No event.
    - [ ] Test: Listener present = Event received.
    - [ ] Test: Data accuracy (position, normal).
    - [ ] Test: Thread safety (stress test).

## 📚 Usage Example

```cpp
// 1. Add Listener to an entity
entity.AddComponent<Physics::Component::CollisionListener>(
    Physics::Component::CollisionListener::Default()
);

// 2. Subscribe to events
eventManager.RegisterCallback<Physics::Event::CollisionAddedEvent>(
    [](Engine::Core& core, const Physics::Event::CollisionAddedEvent& event) {
        auto other = event.GetOther(myEntity);
        Logger::Info("Bonk! Hit at position: {}", event.contact.position);

        // Spawn particle at contact point
        Particles::Spawn(event.contact.position, event.contact.normal);
    }
);
```
