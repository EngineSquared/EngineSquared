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

- ✅ Implement Trigger component (sensor collider)
- ✅ Implement TriggerEnter event
- ✅ Implement TriggerExit event
- ✅ No physical collision (objects pass through)
- ✅ Layer-based filtering
- ✅ ECS event-driven architecture
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Trigger.hpp                 # New file
│   ├── TriggerEvent.hpp            # New file
│   └── TriggerListener.hpp         # New file (opt-in)
├── system/
│   ├── TriggerSystem.hpp           # New file
│   └── TriggerSystem.cpp           # New file
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
 * **Differences from regular collider:**
 * - ✅ Detects overlaps
 * - ❌ No collision response (objects pass through)
 * - ❌ No forces/impulses applied
 * - ✅ TriggerEnter/Exit events
 * 
 * **Common uses:**
 * - Pickup items (coins, health packs)
 * - Zone triggers (checkpoint, level exit)
 * - Proximity sensors (enemy detection)
 * - Teleporters
 * - Damage volumes (non-physical)
 * 
 * @note Requires a collider component (Box, Sphere, Capsule, etc.)
 * @note Body must be Static or Kinematic (not Dynamic)
 * 
 * @example
 * @code
 * // Create pickup item (coin)
 * auto coin = core.CreateEntity();
 * 
 * // Static body (triggers must be static/kinematic)
 * Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
 * coin.AddComponent<Physics::Component::RigidBody>(core, rb);
 * 
 * // Sphere collider
 * Physics::Component::SphereCollider collider(0.5f);
 * coin.AddComponent<Physics::Component::SphereCollider>(core, collider);
 * 
 * // Mark as trigger
 * Physics::Component::Trigger trigger;
 * coin.AddComponent<Physics::Component::Trigger>(core, trigger);
 * 
 * // Opt-in to trigger events
 * Physics::Component::TriggerListener listener;
 * coin.AddComponent<Physics::Component::TriggerListener>(core, listener);
 * @endcode
 */
struct Trigger {
    /// Layer mask (which layers can trigger)
    uint32_t layerMask = 0xFFFFFFFF;  ///< Collide with all layers by default
    
    /**
     * @brief Create default trigger
     * @return Trigger that responds to all layers
     */
    static Trigger Default() {
        return Trigger{0xFFFFFFFF};
    }
    
    /**
     * @brief Create trigger for specific layers
     * @param mask Layer mask
     * @return Trigger with custom layer mask
     */
    static Trigger WithLayers(uint32_t mask) {
        return Trigger{mask};
    }
    
    /**
     * @brief Create trigger for character layer only
     * @return Trigger that only responds to characters
     */
    static Trigger CharacterOnly() {
        return Trigger{1u << CollisionLayers::CHARACTER};
    }
    
    /**
     * @brief Check if layer can trigger
     * @param layer Layer index
     * @return true if layer enabled
     */
    bool CanTriggerLayer(uint32_t layer) const {
        return (layerMask & (1u << layer)) != 0;
    }
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

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Trigger enter event (overlap begins)
 * 
 * Fired when an entity enters a trigger volume.
 * 
 * **Use cases:**
 * - Pickup items
 * - Checkpoint activation
 * - Enemy aggro radius
 * - Zone entry notifications
 * 
 * @example
 * @code
 * void OnTriggerEnter(Engine::Core &core,
 *                     const Physics::Component::TriggerEnterEvent &event) {
 *     auto other = event.other;
 *     
 *     // Check if player picked up coin
 *     if (event.self.HasComponent<Game::CoinTag>(core) &&
 *         other.HasComponent<Game::PlayerTag>(core)) {
 *         // Award points
 *         auto &score = other.GetComponent<Game::Score>(core);
 *         score.AddPoints(10);
 *         
 *         // Destroy coin
 *         core.DestroyEntity(event.self);
 *     }
 * }
 * 
 * // Register listener
 * core.GetRegistry().on_construct<Physics::Component::TriggerEnterEvent>()
 *     .connect<&OnTriggerEnter>(core);
 * @endcode
 */
struct TriggerEnterEvent {
    Engine::Entity self;   ///< Trigger entity
    Engine::Entity other;  ///< Entity entering trigger
};

/**
 * @brief Trigger exit event (overlap ends)
 * 
 * Fired when an entity exits a trigger volume.
 * 
 * **Use cases:**
 * - Zone exit notifications
 * - Reset trigger state
 * - Stop continuous effects
 * - Track time in zone
 */
struct TriggerExitEvent {
    Engine::Entity self;   ///< Trigger entity
    Engine::Entity other;  ///< Entity exiting trigger
};

} // namespace Physics::Component
```

### TriggerListener Component (Opt-in)

```cpp
/**************************************************************************
 * @file TriggerListener.hpp
 * @brief Opt-in component for trigger event filtering
 **************************************************************************/

#pragma once

#include <cstdint>

namespace Physics::Component {

/**
 * @brief Trigger listener configuration
 * 
 * Add this component to trigger entities that should fire events.
 * Without this component, trigger events are NOT fired (performance).
 * 
 * @example
 * @code
 * // Listen to trigger events
 * Physics::Component::TriggerListener listener;
 * trigger.AddComponent<Physics::Component::TriggerListener>(core, listener);
 * @endcode
 */
struct TriggerListener {
    /// Event flags
    bool listenEnter = true;   ///< Fire TriggerEnterEvent
    bool listenExit = true;    ///< Fire TriggerExitEvent
    
    /// Filtering
    uint32_t layerMask = 0xFFFFFFFF;  ///< Only trigger for these layers
    
    /**
     * @brief Create default listener (Enter + Exit)
     * @return TriggerListener
     */
    static TriggerListener Default() {
        return TriggerListener{true, true, 0xFFFFFFFF};
    }
    
    /**
     * @brief Create enter-only listener
     * @return TriggerListener (no exit events)
     */
    static TriggerListener EnterOnly() {
        return TriggerListener{true, false, 0xFFFFFFFF};
    }
    
    /**
     * @brief Create listener for specific layers
     * @param mask Layer mask
     * @return TriggerListener with custom layers
     */
    static TriggerListener WithLayers(uint32_t mask) {
        return TriggerListener{true, true, mask};
    }
    
    /**
     * @brief Check if should fire event
     * @param otherLayer Other entity's layer
     * @return true if event should fire
     */
    bool ShouldFireEvent(uint32_t otherLayer) const {
        return (layerMask & (1u << otherLayer)) != 0;
    }
};

} // namespace Physics::Component
```

### TriggerSystem

```cpp
/**************************************************************************
 * @file TriggerSystem.hpp
 * @brief System for managing trigger events
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::System {

/**
 * @brief Trigger system
 * 
 * Manages trigger event lifecycle:
 * 1. Mark bodies with Trigger component as sensors in Jolt
 * 2. Listen to Jolt sensor callbacks
 * 3. Filter by TriggerListener component
 * 4. Create event components (Enter/Exit)
 * 5. Dispatch via entt signals
 * 6. Cleanup old events
 * 
 * **Performance:**
 * - Only entities with TriggerListener receive events
 * - No physics response overhead (sensors are free)
 */
class TriggerSystem {
public:
    /**
     * @brief Initialize trigger system
     * @param core Engine core
     * 
     * Registers Jolt sensor listener.
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown trigger system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Update trigger events (called after physics step)
     * @param core Engine core
     * 
     * Processes pending triggers and dispatches events.
     */
    static void Update(Engine::Core &core);
    
    /**
     * @brief Cleanup old events (called end of frame)
     * @param core Engine core
     * 
     * Removes event components from entities.
     */
    static void Cleanup(Engine::Core &core);
    
    /**
     * @brief Apply trigger component to Jolt body
     * @param core Engine core
     * @param entity Entity with Trigger component
     * 
     * Marks Jolt body as sensor (IsSensor = true).
     */
    static void ApplyTrigger(Engine::Core &core, Engine::Entity entity);

private:
    /// Jolt contact listener for sensors
    class JoltSensorListener;
    
    /// Track active triggers (for exit detection)
    struct TriggerTracker;
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Create Trigger Component (1-2 hours)

- [ ] Create `component/Trigger.hpp`
- [ ] Implement Trigger struct
- [ ] Add layerMask field
- [ ] Implement factory methods
- [ ] Add CanTriggerLayer() method
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create TriggerEvent Components (1-2 hours)

- [ ] Create `component/TriggerEvent.hpp`
- [ ] Implement TriggerEnterEvent
- [ ] Implement TriggerExitEvent
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 3: Create TriggerListener Component (1-2 hours)

- [ ] Create `component/TriggerListener.hpp`
- [ ] Implement TriggerListener struct
- [ ] Add event flags (Enter/Exit)
- [ ] Add layer mask filtering
- [ ] Implement factory methods
- [ ] Add to `Physics.hpp`

### Phase 4: Implement TriggerSystem (6-8 hours)

- [ ] Create `system/TriggerSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement ApplyTrigger()
  - Set Jolt body as sensor (IsSensor = true)
  - Ensure body is Static/Kinematic
- [ ] Create JoltSensorListener
  - Override OnContactAdded (Enter)
  - Override OnContactRemoved (Exit)
- [ ] Implement trigger tracking
  - Store active trigger pairs
  - Detect Enter/Exit state changes
- [ ] Implement event filtering
  - Check TriggerListener component
  - Apply layer mask
- [ ] Implement event dispatch
  - Create event components
  - Fire entt signals
- [ ] Implement Update() (process pending)
- [ ] Implement Cleanup() (remove old events)

### Phase 5: Integrate with RigidBodySystem (2-3 hours)

- [ ] Update RigidBodySystem to detect Trigger component
- [ ] Call TriggerSystem::ApplyTrigger() when Trigger added
- [ ] Ensure body is Static/Kinematic (reject Dynamic)
- [ ] Set Jolt IsSensor flag

### Phase 6: Unit Tests (5-6 hours)

- [ ] Create `tests/TriggerSystemTest.cpp`
- [ ] Test TriggerEnterEvent fired
- [ ] Test TriggerExitEvent fired
- [ ] Test no physical collision (objects pass through)
- [ ] Test layer filtering
- [ ] Test TriggerListener opt-in
- [ ] Test Dynamic body rejected
- [ ] Test multiple entities in trigger
- [ ] Test trigger without listener (no events)

### Phase 7: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Pickup Item" example (coin)
- [ ] Add "Checkpoint" example (zone trigger)
- [ ] Add "Enemy Aggro" example (proximity sensor)
- [ ] Add "Teleporter" example (zone transition)

### Phase 8: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Triggers" section
  - Explain triggers vs colliders
  - Performance benefits
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] TriggerEnter fired when entering
- [ ] TriggerExit fired when exiting
- [ ] No physical collision (pass through)
- [ ] Layer filtering works
- [ ] Only Static/Kinematic allowed
- [ ] Dynamic body rejected
- [ ] No events without TriggerListener
- [ ] Multiple entities tracked correctly
- [ ] Events cleaned up properly

### Integration Tests

- [ ] Pickup items collected
- [ ] Checkpoint activation works
- [ ] Aggro radius detection
- [ ] Teleporter zone transition

### Performance Tests

- [ ] 100 triggers: < 3ms event dispatch
- [ ] 1000 triggers: < 30ms
- [ ] No physics overhead (sensors are free)

### Edge Cases

- [ ] Trigger with deleted entity
- [ ] Multiple entities entering/exiting
- [ ] Trigger without collider
- [ ] Overlapping triggers

## 📚 Code Examples

### Example 1: Pickup Item (Coin)

```cpp
// Create collectible coin
void CreateCoin(Engine::Core &core, glm::vec3 position) {
    auto coin = core.CreateEntity();
    
    // Transform
    coin.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Visual mesh
    Object::Component::Mesh mesh = Object::Resource::CreateSphere(core, 0.3f);
    coin.AddComponent<Object::Component::Mesh>(core, mesh);
    
    // Physics: static body
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
    coin.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Sphere collider
    Physics::Component::SphereCollider collider(0.5f);
    coin.AddComponent<Physics::Component::SphereCollider>(core, collider);
    
    // Mark as trigger (no physical collision)
    Physics::Component::Trigger trigger = Physics::Component::Trigger::CharacterOnly();
    coin.AddComponent<Physics::Component::Trigger>(core, trigger);
    
    // Opt-in to trigger events (enter only, no exit needed)
    Physics::Component::TriggerListener listener = 
        Physics::Component::TriggerListener::EnterOnly();
    coin.AddComponent<Physics::Component::TriggerListener>(core, listener);
    
    // Tag for identification
    coin.AddComponent<Game::CoinTag>(core);
}

// Handle coin pickup
void OnCoinPickup(Engine::Core &core,
                  const Physics::Component::TriggerEnterEvent &event) {
    // Check if coin and player
    if (!event.self.HasComponent<Game::CoinTag>(core)) return;
    if (!event.other.HasComponent<Game::PlayerTag>(core)) return;
    
    // Award points
    auto &score = event.other.GetComponent<Game::Score>(core);
    score.AddPoints(10);
    
    // Play sound
    Audio::PlaySound("sounds/coin_pickup.wav");
    
    // Spawn particles
    auto &transform = event.self.GetComponent<Object::Component::Transform>(core);
    Particles::SpawnBurst(core, transform.position, glm::vec3(0, 1, 0), 20);
    
    // Destroy coin
    core.DestroyEntity(event.self);
    
    Logger::Info("Player collected coin! Score: {}", score.points);
}

// Register handler
void SetupPickupSystem(Engine::Core &core) {
    core.GetRegistry().on_construct<Physics::Component::TriggerEnterEvent>()
        .connect<&OnCoinPickup>(core);
}
```

### Example 2: Checkpoint System

```cpp
// Checkpoint trigger zone
void CreateCheckpoint(Engine::Core &core, glm::vec3 position, int checkpointId) {
    auto checkpoint = core.CreateEntity();
    
    // Transform
    checkpoint.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Physics: static body
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
    checkpoint.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Box collider (large zone)
    Physics::Component::BoxCollider collider(glm::vec3(2.0f, 5.0f, 2.0f));
    checkpoint.AddComponent<Physics::Component::BoxCollider>(core, collider);
    
    // Trigger (character only)
    Physics::Component::Trigger trigger = Physics::Component::Trigger::CharacterOnly();
    checkpoint.AddComponent<Physics::Component::Trigger>(core, trigger);
    
    // Listener
    Physics::Component::TriggerListener listener;
    checkpoint.AddComponent<Physics::Component::TriggerListener>(core, listener);
    
    // Checkpoint data
    Game::CheckpointData data{checkpointId, false};
    checkpoint.AddComponent<Game::CheckpointData>(core, data);
}

// Checkpoint activation
void OnCheckpointReached(Engine::Core &core,
                         const Physics::Component::TriggerEnterEvent &event) {
    if (!event.self.HasComponent<Game::CheckpointData>(core)) return;
    if (!event.other.HasComponent<Game::PlayerTag>(core)) return;
    
    auto &data = event.self.GetComponent<Game::CheckpointData>(core);
    
    if (!data.activated) {
        // Activate checkpoint
        data.activated = true;
        
        // Save player position
        auto &playerTransform = event.other.GetComponent<Object::Component::Transform>(core);
        Game::SaveSystem::SetRespawnPoint(playerTransform.position);
        
        // Visual feedback
        Logger::Info("Checkpoint {} activated!", data.id);
        Audio::PlaySound("sounds/checkpoint.wav");
        
        // Spawn effect
        Particles::SpawnCheckpointEffect(core, playerTransform.position);
    }
}
```

### Example 3: Enemy Aggro Radius

```cpp
// Enemy with detection radius
void CreateEnemy(Engine::Core &core, glm::vec3 position) {
    auto enemy = core.CreateEntity();
    
    // ... setup enemy transform, mesh, AI, etc ...
    
    // Aggro trigger (separate entity, child of enemy)
    auto aggroTrigger = core.CreateEntity();
    
    // Position at enemy location
    aggroTrigger.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Physics: static body (kinematic if enemy moves)
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
    aggroTrigger.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Large sphere collider (detection radius)
    Physics::Component::SphereCollider collider(10.0f);  // 10 unit radius
    aggroTrigger.AddComponent<Physics::Component::SphereCollider>(core, collider);
    
    // Trigger (character layer only)
    Physics::Component::Trigger trigger = Physics::Component::Trigger::CharacterOnly();
    aggroTrigger.AddComponent<Physics::Component::Trigger>(core, trigger);
    
    // Listener
    Physics::Component::TriggerListener listener;
    aggroTrigger.AddComponent<Physics::Component::TriggerListener>(core, listener);
    
    // Link trigger to enemy
    Game::AggroTrigger aggroData{enemy};
    aggroTrigger.AddComponent<Game::AggroTrigger>(core, aggroData);
}

// Aggro detection
void OnEnemyAggro(Engine::Core &core,
                  const Physics::Component::TriggerEnterEvent &event) {
    if (!event.self.HasComponent<Game::AggroTrigger>(core)) return;
    if (!event.other.HasComponent<Game::PlayerTag>(core)) return;
    
    // Get enemy entity
    auto &aggroData = event.self.GetComponent<Game::AggroTrigger>(core);
    auto enemy = aggroData.enemyEntity;
    
    // Activate enemy AI
    if (enemy.HasComponent<Game::EnemyAI>(core)) {
        auto &ai = enemy.GetComponent<Game::EnemyAI>(core);
        ai.SetTarget(event.other);
        ai.SetState(Game::EnemyState::CHASING);
        
        Logger::Info("Enemy detected player!");
    }
}

// Deaggro
void OnEnemyDeaggro(Engine::Core &core,
                    const Physics::Component::TriggerExitEvent &event) {
    if (!event.self.HasComponent<Game::AggroTrigger>(core)) return;
    
    auto &aggroData = event.self.GetComponent<Game::AggroTrigger>(core);
    auto enemy = aggroData.enemyEntity;
    
    if (enemy.HasComponent<Game::EnemyAI>(core)) {
        auto &ai = enemy.GetComponent<Game::EnemyAI>(core);
        ai.ClearTarget();
        ai.SetState(Game::EnemyState::PATROLLING);
        
        Logger::Info("Enemy lost player!");
    }
}
```

### Example 4: Teleporter

```cpp
// Teleporter entrance
void CreateTeleporter(Engine::Core &core, glm::vec3 position, glm::vec3 destination) {
    auto teleporter = core.CreateEntity();
    
    // Transform
    teleporter.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Visual effect (portal mesh)
    // ... mesh setup ...
    
    // Physics: static body
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
    teleporter.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Box collider (portal size)
    Physics::Component::BoxCollider collider(glm::vec3(1.0f, 2.0f, 0.5f));
    teleporter.AddComponent<Physics::Component::BoxCollider>(core, collider);
    
    // Trigger
    Physics::Component::Trigger trigger = Physics::Component::Trigger::Default();
    teleporter.AddComponent<Physics::Component::Trigger>(core, trigger);
    
    // Listener (enter only)
    Physics::Component::TriggerListener listener =
        Physics::Component::TriggerListener::EnterOnly();
    teleporter.AddComponent<Physics::Component::TriggerListener>(core, listener);
    
    // Teleporter data
    Game::TeleporterData data{destination};
    teleporter.AddComponent<Game::TeleporterData>(core, data);
}

// Teleportation
void OnTeleport(Engine::Core &core,
                const Physics::Component::TriggerEnterEvent &event) {
    if (!event.self.HasComponent<Game::TeleporterData>(core)) return;
    
    auto &data = event.self.GetComponent<Game::TeleporterData>(core);
    auto &transform = event.other.GetComponent<Object::Component::Transform>(core);
    
    // Teleport entity
    transform.position = data.destination;
    
    // Visual/audio feedback
    Particles::SpawnTeleportEffect(core, transform.position);
    Audio::PlaySound("sounds/teleport.wav");
    
    Logger::Info("Entity teleported to: {}, {}, {}",
                 data.destination.x, data.destination.y, data.destination.z);
}
```

### Example 5: Zone Tracking (Multiple Entities)

```cpp
// Track all entities in a zone
struct ZoneTracker {
    std::unordered_set<Engine::Entity> entitiesInZone;
    
    void OnEnter(Engine::Core &core,
                 const Physics::Component::TriggerEnterEvent &event) {
        if (!IsZoneTrigger(event.self, core)) return;
        
        entitiesInZone.insert(event.other);
        
        Logger::Info("Entity {} entered zone (total: {})",
                     entt::to_integral(event.other), entitiesInZone.size());
        
        // Game logic based on count
        if (entitiesInZone.size() >= 3) {
            // Minimum 3 players required
            StartMinigame(core);
        }
    }
    
    void OnExit(Engine::Core &core,
                const Physics::Component::TriggerExitEvent &event) {
        if (!IsZoneTrigger(event.self, core)) return;
        
        entitiesInZone.erase(event.other);
        
        Logger::Info("Entity {} exited zone (total: {})",
                     entt::to_integral(event.other), entitiesInZone.size());
        
        if (entitiesInZone.size() < 3) {
            // Not enough players
            EndMinigame(core);
        }
    }
    
    bool IsZoneTrigger(Engine::Entity entity, Engine::Core &core) {
        return entity.HasComponent<Game::ZoneTriggerTag>(core);
    }
};
```

## 🐛 Error Handling

### Cases to Handle

1. **Dynamic body with Trigger**
   ```cpp
   if (rb.motionType == MotionType::Dynamic) {
       Logger::Error("Trigger: Dynamic bodies not supported (use Static or Kinematic)");
       return;
   }
   ```

2. **Trigger without collider**
   ```cpp
   if (!HasAnyCollider(entity, core)) {
       Logger::Error("Trigger: Entity has no collider component");
       return;
   }
   ```

3. **Trigger without listener**
   ```cpp
   if (!entity.HasComponent<TriggerListener>(core)) {
       Logger::Warn("Trigger: No TriggerListener (events disabled)");
   }
   ```

4. **Entity destroyed during trigger**
   ```cpp
   void OnTriggerEnter(Core &core, const TriggerEnterEvent &event) {
       if (!event.other.IsValid(core)) {
           Logger::Warn("Trigger: Entity destroyed");
           return;
       }
   }
   ```

## 💡 Design Considerations

### Triggers vs Collision Events

| Feature | Trigger | Collision Event |
|---------|---------|-----------------|
| **Physical Response** | ❌ No | ✅ Yes |
| **Performance** | Fast (sensor) | Slower (full collision) |
| **Use Case** | Detection only | Gameplay + physics |
| **Events** | TriggerEnter/Exit | CollisionEnter/Stay/Exit |

**When to use Trigger:**
- ✅ Pickups, checkpoints, zones
- ✅ Proximity detection
- ✅ No physical interaction needed

**When to use Collision:**
- ✅ Damage from impact
- ✅ Physics-based gameplay
- ✅ Realistic collisions

### Static vs Kinematic Triggers

```cpp
// ✅ Static trigger (immobile)
auto rb = RigidBody::CreateStatic();

// ✅ Kinematic trigger (can move)
auto rb = RigidBody::CreateKinematic();
// Move with MoveKinematic()

// ❌ Dynamic trigger (NOT SUPPORTED)
auto rb = RigidBody::CreateDynamic(10.0f);  // ERROR
```

### Enter-Only vs Enter+Exit

```cpp
// Enter-only (pickups, one-time triggers)
auto listener = TriggerListener::EnterOnly();

// Enter+Exit (zones, tracking)
auto listener = TriggerListener::Default();
```

## 📊 Success Criteria

- [ ] Trigger component implemented
- [ ] TriggerEvent components implemented
- [ ] TriggerListener component implemented
- [ ] TriggerSystem implemented
- [ ] Jolt sensor integration working
- [ ] Enter/Exit events fired correctly
- [ ] No physical collision (pass through)
- [ ] Layer filtering works
- [ ] Opt-in architecture (no listener = no events)
- [ ] Dynamic body rejected
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 3ms for 100 triggers)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Sensors: https://jrouwe.github.io/JoltPhysics/
- Unity OnTriggerEnter: https://docs.unity3d.com/ScriptReference/Collider.OnTriggerEnter.html
- Godot area_entered signal: https://docs.godotengine.org/en/stable/classes/class_area3d.html#signals
- Unreal OnComponentBeginOverlap: https://docs.unrealengine.com/5.0/en-US/BlueprintAPI/Collision/OnComponentBeginOverlap/

## 🚀 Follow-up Issues

- #015: Basic Constraints (Fixed, Distance, Point)
- #020: Debug Visualization (draw trigger volumes)
- #024: Performance Profiling (trigger statistics)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.6`, `priority:high`, `triggers`  
**Milestone:** v0.6 - Collision Events & Constraints
