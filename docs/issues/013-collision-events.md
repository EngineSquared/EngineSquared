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

- ✅ Implement CollisionEnter event (first contact)
- ✅ Implement CollisionStay event (continuous contact)
- ✅ Implement CollisionExit event (separation)
- ✅ Provide detailed collision information
- ✅ ECS event-driven architecture
- ✅ Performance-optimized event dispatch
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── CollisionEvent.hpp          # New file
│   └── CollisionListener.hpp       # New file (opt-in component)
├── system/
│   ├── CollisionEventSystem.hpp    # New file
│   └── CollisionEventSystem.cpp    # New file
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

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Contact point information
 * 
 * Describes a single point of contact between two bodies.
 */
struct ContactPoint {
    glm::vec3 point = glm::vec3(0.0f);         ///< Contact point in world space
    glm::vec3 normal = glm::vec3(0.0f);        ///< Contact normal (from A to B)
    float penetrationDepth = 0.0f;             ///< Penetration depth
    glm::vec3 localPointA = glm::vec3(0.0f);   ///< Contact point in body A local space
    glm::vec3 localPointB = glm::vec3(0.0f);   ///< Contact point in body B local space
    float impulse = 0.0f;                       ///< Impulse magnitude applied
    
    /**
     * @brief Get separation velocity at contact
     * @return Relative velocity along normal
     */
    float GetSeparationVelocity() const;
};

/**
 * @brief Collision data shared by all event types
 */
struct CollisionData {
    Engine::Entity entityA;                    ///< First entity
    Engine::Entity entityB;                    ///< Second entity
    std::vector<ContactPoint> contacts;        ///< Contact points (1-4 typically)
    glm::vec3 relativeVelocity = glm::vec3(0.0f);  ///< Relative velocity at collision
    float totalImpulse = 0.0f;                 ///< Total impulse magnitude
    
    /**
     * @brief Get other entity from perspective of self
     * @param self Current entity
     * @return Other entity in collision
     */
    Engine::Entity GetOther(Engine::Entity self) const {
        return (self == entityA) ? entityB : entityA;
    }
    
    /**
     * @brief Get primary contact point (highest impulse)
     * @return Main contact point
     */
    const ContactPoint& GetMainContact() const {
        if (contacts.empty()) {
            static ContactPoint empty;
            return empty;
        }
        
        // Find contact with highest impulse
        const ContactPoint *main = &contacts[0];
        for (const auto &contact : contacts) {
            if (contact.impulse > main->impulse) {
                main = &contact;
            }
        }
        return *main;
    }
    
    /**
     * @brief Check if collision is significant (not trivial)
     * @param threshold Impulse threshold
     * @return true if impulse exceeds threshold
     */
    bool IsSignificant(float threshold = 0.1f) const {
        return totalImpulse >= threshold;
    }
};

/**
 * @brief Collision enter event (first contact)
 * 
 * Fired once when two bodies start touching.
 * 
 * **Use cases:**
 * - Damage on impact
 * - Sound effects
 * - Particle effects
 * - Score tracking
 * - Game state changes
 * 
 * @example
 * @code
 * void OnCollisionEnter(Engine::Core &core, const Physics::Component::CollisionEnterEvent &event) {
 *     auto other = event.data.GetOther(event.self);
 *     Logger::Info("Entity {} collided with {}", 
 *                  entt::to_integral(event.self),
 *                  entt::to_integral(other));
 * }
 * 
 * // Register listener
 * core.GetRegistry().on_construct<Physics::Component::CollisionEnterEvent>()
 *     .connect<&OnCollisionEnter>(core);
 * @endcode
 */
struct CollisionEnterEvent {
    Engine::Entity self;      ///< Entity receiving event
    CollisionData data;       ///< Collision details
};

/**
 * @brief Collision stay event (continuous contact)
 * 
 * Fired every physics frame while bodies remain in contact.
 * 
 * **Use cases:**
 * - Continuous damage (lava, spikes)
 * - Friction effects
 * - Sliding sounds
 * - Pressure plates
 * 
 * @note More frequent than Enter/Exit, use sparingly
 */
struct CollisionStayEvent {
    Engine::Entity self;      ///< Entity receiving event
    CollisionData data;       ///< Collision details
};

/**
 * @brief Collision exit event (separation)
 * 
 * Fired once when two bodies stop touching.
 * 
 * **Use cases:**
 * - Reset state after contact
 * - Stop continuous effects
 * - Track time in contact
 * - Cleanup
 */
struct CollisionExitEvent {
    Engine::Entity self;      ///< Entity receiving event
    Engine::Entity other;     ///< Other entity in collision
};

} // namespace Physics::Component
```

### CollisionListener Component (Opt-in)

```cpp
/**************************************************************************
 * @file CollisionListener.hpp
 * @brief Opt-in component for collision event filtering
 **************************************************************************/

#pragma once

#include <cstdint>

namespace Physics::Component {

/**
 * @brief Collision listener configuration
 * 
 * Add this component to entities that should receive collision events.
 * Without this component, collision events are NOT fired (performance).
 * 
 * **Event filtering:**
 * - Enable/disable specific event types
 * - Impulse threshold (ignore trivial collisions)
 * - Layer mask (only collide with specific layers)
 * 
 * @example
 * @code
 * // Listen to all collision events
 * Physics::Component::CollisionListener listener;
 * entity.AddComponent<Physics::Component::CollisionListener>(core, listener);
 * 
 * // Listen only to significant impacts (damage threshold)
 * Physics::Component::CollisionListener listener;
 * listener.minImpulse = 10.0f;  // Ignore soft touches
 * entity.AddComponent<Physics::Component::CollisionListener>(core, listener);
 * @endcode
 */
struct CollisionListener {
    /// Event flags
    bool listenEnter = true;   ///< Fire CollisionEnterEvent
    bool listenStay = false;   ///< Fire CollisionStayEvent (use sparingly!)
    bool listenExit = true;    ///< Fire CollisionExitEvent
    
    /// Filtering
    float minImpulse = 0.0f;   ///< Minimum impulse to trigger event (0 = all)
    uint32_t layerMask = 0xFFFFFFFF;  ///< Only collide with these layers
    
    /**
     * @brief Create default listener (Enter + Exit only)
     * @return CollisionListener
     */
    static CollisionListener Default() {
        return CollisionListener{true, false, true, 0.0f, 0xFFFFFFFF};
    }
    
    /**
     * @brief Create listener for all events
     * @return CollisionListener with Stay enabled
     */
    static CollisionListener All() {
        return CollisionListener{true, true, true, 0.0f, 0xFFFFFFFF};
    }
    
    /**
     * @brief Create listener for impacts only (high impulse)
     * @param minImpulseThreshold Minimum impulse
     * @return CollisionListener
     */
    static CollisionListener ImpactsOnly(float minImpulseThreshold = 5.0f) {
        return CollisionListener{true, false, false, minImpulseThreshold, 0xFFFFFFFF};
    }
    
    /**
     * @brief Check if should fire event
     * @param impulse Collision impulse
     * @param otherLayer Other entity's layer
     * @return true if event should fire
     */
    bool ShouldFireEvent(float impulse, uint32_t otherLayer) const {
        // Check impulse threshold
        if (impulse < minImpulse) return false;
        
        // Check layer mask
        if ((layerMask & (1u << otherLayer)) == 0) return false;
        
        return true;
    }
};

} // namespace Physics::Component
```

### CollisionEventSystem

```cpp
/**************************************************************************
 * @file CollisionEventSystem.hpp
 * @brief System for managing collision events
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::System {

/**
 * @brief Collision event system
 * 
 * Manages collision event lifecycle:
 * 1. Listen to Jolt contact callbacks
 * 2. Filter by CollisionListener component
 * 3. Create event components (Enter/Stay/Exit)
 * 4. Dispatch via entt signals
 * 5. Cleanup old events
 * 
 * **Performance:**
 * - Only entities with CollisionListener receive events
 * - Events are pooled and reused
 * - Cleanup happens after frame
 */
class CollisionEventSystem {
public:
    /**
     * @brief Initialize collision event system
     * @param core Engine core
     * 
     * Registers Jolt contact listener.
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown collision event system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Update collision events (called after physics step)
     * @param core Engine core
     * 
     * Processes pending collisions and dispatches events.
     */
    static void Update(Engine::Core &core);
    
    /**
     * @brief Cleanup old events (called end of frame)
     * @param core Engine core
     * 
     * Removes event components from entities.
     */
    static void Cleanup(Engine::Core &core);

private:
    /// Jolt contact listener implementation
    class JoltContactListener;
    
    /// Track active collisions (for Stay/Exit detection)
    struct CollisionTracker;
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Create Event Components (3-4 hours)

- [ ] Create `component/CollisionEvent.hpp`
- [ ] Implement ContactPoint struct
- [ ] Implement CollisionData struct
- [ ] Implement CollisionEnterEvent
- [ ] Implement CollisionStayEvent
- [ ] Implement CollisionExitEvent
- [ ] Add helper methods (GetOther, GetMainContact, IsSignificant)
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create CollisionListener Component (2-3 hours)

- [ ] Create `component/CollisionListener.hpp`
- [ ] Implement CollisionListener struct
- [ ] Add event flags (Enter/Stay/Exit)
- [ ] Add filtering (minImpulse, layerMask)
- [ ] Implement factory methods
- [ ] Add ShouldFireEvent() method
- [ ] Add to `Physics.hpp`

### Phase 3: Implement CollisionEventSystem (8-10 hours)

- [ ] Create `system/CollisionEventSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Create JoltContactListener implementation
  - Override OnContactAdded (Enter)
  - Override OnContactPersisted (Stay)
  - Override OnContactRemoved (Exit)
- [ ] Implement collision tracking
  - Store active collision pairs
  - Detect Enter/Stay/Exit state changes
- [ ] Implement event filtering
  - Check CollisionListener component
  - Apply impulse threshold
  - Apply layer mask
- [ ] Implement event dispatch
  - Create event components
  - Fire entt signals
- [ ] Implement Update() (process pending)
- [ ] Implement Cleanup() (remove old events)

### Phase 4: Integrate with PhysicsPlugin (3-4 hours)

- [ ] Update PhysicsPlugin to initialize CollisionEventSystem
- [ ] Call Update() after physics step
- [ ] Call Cleanup() at end of frame
- [ ] Register contact listener with Jolt

### Phase 5: Unit Tests (6-7 hours)

- [ ] Create `tests/CollisionEventTest.cpp`
- [ ] Test CollisionEnterEvent fired
- [ ] Test CollisionStayEvent fired
- [ ] Test CollisionExitEvent fired
- [ ] Test event filtering (impulse threshold)
- [ ] Test event filtering (layer mask)
- [ ] Test CollisionListener opt-in (no component = no events)
- [ ] Test GetOther() method
- [ ] Test GetMainContact() method
- [ ] Test multiple contacts per collision

### Phase 6: Integration Examples (5-6 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Damage on Impact" example
  - Reduce health based on impulse
- [ ] Add "Sound Effects" example
  - Play sound on collision
- [ ] Add "Particle Spawn" example
  - Spawn particles at contact point
- [ ] Add "Continuous Damage" example
  - Lava/spikes using CollisionStay

### Phase 7: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Collision Events" section
  - Explain Enter/Stay/Exit lifecycle
  - Performance tips (opt-in, Stay usage)
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] CollisionEnter fired on first contact
- [ ] CollisionStay fired during contact
- [ ] CollisionExit fired on separation
- [ ] Events contain correct entities
- [ ] Contact points are accurate
- [ ] Impulse calculation correct
- [ ] GetOther() returns correct entity
- [ ] GetMainContact() finds highest impulse
- [ ] Impulse threshold filtering works
- [ ] Layer mask filtering works
- [ ] No events without CollisionListener

### Integration Tests

- [ ] Damage system responds to collisions
- [ ] Sound effects triggered correctly
- [ ] Particle spawning works
- [ ] Continuous damage (Stay) works
- [ ] Events cleaned up properly

### Performance Tests

- [ ] 100 collisions: < 5ms event dispatch
- [ ] 1000 collisions: < 50ms
- [ ] Opt-in reduces overhead (no listener = no cost)
- [ ] Stay events don't overwhelm system

### Edge Cases

- [ ] Collision with deleted entity
- [ ] Simultaneous multiple collisions
- [ ] Very high impulse collisions
- [ ] Zero impulse collisions (touching)

## 📚 Code Examples

### Example 1: Damage on Impact

```cpp
// Damage system responds to collision events
void OnCollisionEnter(Engine::Core &core,
                      const Physics::Component::CollisionEnterEvent &event) {
    auto other = event.data.GetOther(event.self);
    
    // Check if collision is significant
    if (!event.data.IsSignificant(5.0f)) {
        return;  // Too soft, no damage
    }
    
    // Apply damage to both entities based on impulse
    if (event.self.HasComponent<Game::Health>(core)) {
        auto &health = event.self.GetComponent<Game::Health>(core);
        float damage = event.data.totalImpulse * 0.5f;
        health.TakeDamage(damage);
        
        Logger::Info("Entity {} took {} damage from impact",
                     entt::to_integral(event.self), damage);
    }
    
    // Spawn impact effect at contact point
    const auto &contact = event.data.GetMainContact();
    SpawnImpactEffect(core, contact.point, contact.normal);
}

// Register event listener
void SetupCollisionHandlers(Engine::Core &core) {
    core.GetRegistry().on_construct<Physics::Component::CollisionEnterEvent>()
        .connect<&OnCollisionEnter>(core);
}

// Enable collision events for player
void CreatePlayer(Engine::Core &core) {
    auto player = core.CreateEntity();
    
    // ... setup transform, mesh, rigidbody, etc ...
    
    // Opt-in to collision events
    Physics::Component::CollisionListener listener = 
        Physics::Component::CollisionListener::Default();
    player.AddComponent<Physics::Component::CollisionListener>(core, listener);
}
```

### Example 2: Sound Effects on Collision

```cpp
// Play sound based on collision intensity
void OnCollisionSound(Engine::Core &core,
                      const Physics::Component::CollisionEnterEvent &event) {
    float impulse = event.data.totalImpulse;
    
    // Select sound based on impact strength
    std::string soundFile;
    if (impulse > 20.0f) {
        soundFile = "sounds/impact_heavy.wav";
    } else if (impulse > 5.0f) {
        soundFile = "sounds/impact_medium.wav";
    } else {
        soundFile = "sounds/impact_light.wav";
    }
    
    // Play at contact point
    const auto &contact = event.data.GetMainContact();
    Audio::PlaySound3D(soundFile, contact.point);
}

// Enable sound listener with impulse threshold
void CreateSoundEmitter(Engine::Core &core, Engine::Entity entity) {
    // Only trigger sound for impacts > 2.0 impulse
    Physics::Component::CollisionListener listener =
        Physics::Component::CollisionListener::ImpactsOnly(2.0f);
    entity.AddComponent<Physics::Component::CollisionListener>(core, listener);
}
```

### Example 3: Particle Effects at Contact

```cpp
// Spawn particles at each contact point
void OnCollisionParticles(Engine::Core &core,
                          const Physics::Component::CollisionEnterEvent &event) {
    // Spawn particles at all contact points
    for (const auto &contact : event.data.contacts) {
        // Scale particle count by impulse
        int particleCount = static_cast<int>(contact.impulse * 10.0f);
        particleCount = std::clamp(particleCount, 5, 50);
        
        // Spawn particles
        Particles::SpawnBurst(
            core,
            contact.point,              // Position
            contact.normal,             // Direction
            particleCount,              // Count
            glm::vec3(1.0f, 0.5f, 0.2f) // Color (orange sparks)
        );
    }
}

// Entity setup
void CreateBreakableObject(Engine::Core &core, glm::vec3 position) {
    auto object = core.CreateEntity();
    
    // ... setup ...
    
    // Listen to collisions for particle effects
    Physics::Component::CollisionListener listener;
    listener.listenEnter = true;
    listener.listenStay = false;   // No particles during contact
    listener.listenExit = false;
    listener.minImpulse = 3.0f;    // Only significant impacts
    object.AddComponent<Physics::Component::CollisionListener>(core, listener);
}
```

### Example 4: Continuous Damage (Lava, Spikes)

```cpp
// Lava that damages on contact and while standing
void OnLavaContact(Engine::Core &core,
                   const Physics::Component::CollisionStayEvent &event) {
    auto other = event.data.GetOther(event.self);
    
    // Damage per physics frame (60Hz = ~0.017s)
    if (other.HasComponent<Game::Health>(core)) {
        auto &health = other.GetComponent<Game::Health>(core);
        float damagePerSecond = 20.0f;
        float damageThisFrame = damagePerSecond / 60.0f;  // Assume 60Hz physics
        health.TakeDamage(damageThisFrame);
    }
}

// Create lava hazard
void CreateLavaPool(Engine::Core &core, glm::vec3 position) {
    auto lava = core.CreateEntity();
    
    // ... setup transform, mesh, static rigidbody, box collider ...
    
    // Enable CollisionStay events (rare case!)
    Physics::Component::CollisionListener listener;
    listener.listenEnter = true;   // Splash effect on entry
    listener.listenStay = true;    // Continuous damage
    listener.listenExit = true;    // Stop burning effect
    lava.AddComponent<Physics::Component::CollisionListener>(core, listener);
    
    // Register Stay event handler
    core.GetRegistry().on_construct<Physics::Component::CollisionStayEvent>()
        .connect<&OnLavaContact>(core);
}
```

### Example 5: Layer-Filtered Events (Projectile vs Enemy)

```cpp
// Projectile that only damages enemies
void CreateProjectile(Engine::Core &core, glm::vec3 position, glm::vec3 velocity) {
    auto projectile = core.CreateEntity();
    
    // ... setup transform, mesh, dynamic rigidbody, sphere collider ...
    
    // Collision listener: only collide with CHARACTER layer
    Physics::Component::CollisionListener listener;
    listener.listenEnter = true;
    listener.listenStay = false;
    listener.listenExit = false;
    listener.minImpulse = 0.0f;    // Even soft touches count
    listener.layerMask = (1u << Physics::Component::CollisionLayers::CHARACTER);
    projectile.AddComponent<Physics::Component::CollisionListener>(core, listener);
}

// Projectile collision handler
void OnProjectileHit(Engine::Core &core,
                     const Physics::Component::CollisionEnterEvent &event) {
    // Check if this is the projectile
    if (!event.self.HasComponent<Game::ProjectileTag>(core)) {
        return;
    }
    
    auto target = event.data.GetOther(event.self);
    
    // Apply damage to target
    if (target.HasComponent<Game::Health>(core)) {
        auto &health = target.GetComponent<Game::Health>(core);
        health.TakeDamage(25.0f);
    }
    
    // Destroy projectile
    core.DestroyEntity(event.self);
}
```

### Example 6: Enter/Exit State Tracking

```cpp
// Track entities in trigger zone
struct TriggerZone {
    std::unordered_set<Engine::Entity> entitiesInZone;
    
    void OnEnter(Engine::Core &core,
                 const Physics::Component::CollisionEnterEvent &event) {
        auto other = event.data.GetOther(event.self);
        entitiesInZone.insert(other);
        
        Logger::Info("Entity {} entered zone (total: {})",
                     entt::to_integral(other), entitiesInZone.size());
        
        // Trigger game logic
        if (entitiesInZone.size() == 1) {
            // First entity entered
            ActivateZone();
        }
    }
    
    void OnExit(Engine::Core &core,
                const Physics::Component::CollisionExitEvent &event) {
        entitiesInZone.erase(event.other);
        
        Logger::Info("Entity {} exited zone (total: {})",
                     entt::to_integral(event.other), entitiesInZone.size());
        
        // Trigger game logic
        if (entitiesInZone.empty()) {
            // Last entity left
            DeactivateZone();
        }
    }
    
    void ActivateZone() {
        Logger::Info("Zone activated!");
        // Open door, spawn enemies, etc.
    }
    
    void DeactivateZone() {
        Logger::Info("Zone deactivated!");
    }
};
```

## 🐛 Error Handling

### Cases to Handle

1. **Entity destroyed during event**
   ```cpp
   void OnCollisionEnter(Core &core, const CollisionEnterEvent &event) {
       auto other = event.data.GetOther(event.self);
       
       // Check validity before accessing
       if (!other.IsValid(core)) {
           Logger::Warn("Collision with destroyed entity");
           return;
       }
       
       // Safe to use
   }
   ```

2. **Listener without event handler**
   ```cpp
   // Warning: CollisionListener added but no handler registered
   if (entity.HasComponent<CollisionListener>(core) &&
       !HasCollisionHandler(core)) {
       Logger::Warn("Entity has CollisionListener but no event handler registered");
   }
   ```

3. **Too many Stay events**
   ```cpp
   // Monitor Stay event frequency
   if (stayEventCount > 1000) {
       Logger::Warn("CollisionStay: {} events this frame (performance impact)",
                    stayEventCount);
   }
   ```

4. **Invalid contact data**
   ```cpp
   if (event.data.contacts.empty()) {
       Logger::Error("CollisionEvent: No contact points");
       return;
   }
   ```

## 💡 Design Considerations

### Enter vs Stay vs Exit

| Event Type | Frequency | Use Case | Performance |
|------------|-----------|----------|-------------|
| **Enter** | Once (first contact) | Damage, sounds, one-time effects | Low |
| **Stay** | Every physics frame | Continuous damage, pressure | HIGH |
| **Exit** | Once (separation) | State reset, cleanup | Low |

**Rule:** Use **Stay** sparingly - it fires 60+ times per second!

### Opt-in Architecture

```cpp
// ❌ BAD: All entities receive events (performance cost)
for (auto entity : allEntities) {
    FireCollisionEvent(entity);
}

// ✅ GOOD: Only entities with CollisionListener
auto view = registry.view<CollisionListener, RigidBody>();
for (auto entity : view) {
    if (ShouldFireEvent(entity)) {
        FireCollisionEvent(entity);
    }
}
```

### Event Cleanup

Events are **temporary components** - they must be cleaned up:
```cpp
// After event handlers run, remove events
registry.clear<CollisionEnterEvent>();
registry.clear<CollisionStayEvent>();
registry.clear<CollisionExitEvent>();
```

### Contact Point Count

Jolt provides up to **4 contact points** per collision.
- Simple shapes: 1-2 contacts
- Complex shapes: 3-4 contacts
- Use `GetMainContact()` for primary contact

## 📊 Success Criteria

- [ ] CollisionEvent components implemented
- [ ] CollisionListener component implemented
- [ ] CollisionEventSystem implemented
- [ ] Jolt contact listener integrated
- [ ] Enter/Stay/Exit events fired correctly
- [ ] Event filtering works (impulse, layers)
- [ ] Opt-in architecture (no listener = no events)
- [ ] Events cleaned up properly
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 100 collisions)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Contact Listener: https://jrouwe.github.io/JoltPhysics/class_contact_listener.html
- Unity OnCollisionEnter: https://docs.unity3d.com/ScriptReference/Collider.OnCollisionEnter.html
- Godot body_entered signal: https://docs.godotengine.org/en/stable/classes/class_rigidbody3d.html#signals
- Unreal OnComponentHit: https://docs.unrealengine.com/5.0/en-US/BlueprintAPI/Collision/OnComponentHit/

## 🚀 Follow-up Issues

- #014: Trigger Events (overlap without physical collision)
- #018: Contact Modification (change friction/restitution mid-collision)
- #020: Debug Visualization (draw contact points)
- #024: Performance Profiling (event statistics)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.6`, `priority:high`, `events`  
**Milestone:** v0.6 - Collision Events & Constraints
