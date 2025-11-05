# Issue #012: Implement Advanced Collision Filtering

**Milestone:** v0.5 - Collision Queries  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 3-4 days  
**Dependencies:** #009 (Raycast), #011 (Overlap)  
**Related Issues:** #004 (RigidBody Properties)

---

## 📋 Description

Implement comprehensive collision filtering system using layers, groups, and custom logic. Essential for controlling which objects collide with each other (player vs enemy, projectiles vs walls, etc.).

Advanced filtering prevents unwanted collisions, improves performance by skipping impossible pairs, and enables complex game mechanics.

## 🎯 Goals

- ✅ Implement collision layer system (32 layers)
- ✅ Implement collision group system (16 groups)
- ✅ Support layer masks for selective collision
- ✅ Custom collision filter callbacks
- ✅ Performance-optimized broad-phase filtering
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── CollisionLayers.hpp         # Extend from #009
│   ├── CollisionFilter.hpp         # New file
│   └── RigidBody.hpp               # Add filter support
├── system/
│   ├── CollisionFilterSystem.hpp   # New file
│   └── CollisionFilterSystem.cpp   # New file
├── resource/
│   └── CollisionMatrix.hpp         # New file (layer matrix)
└── tests/
    ├── CollisionFilterTest.cpp     # New file
    └── CollisionMatrixTest.cpp     # New file
```

### Extend CollisionLayers

```cpp
// Update component/CollisionLayers.hpp from #009
namespace Physics::Component {

/**
 * @brief Extended collision layers with group support
 * 
 * **Layers (32 total):**
 * - Each body belongs to ONE layer (0-31)
 * - Each body has a mask defining which layers to collide with
 * 
 * **Groups (16 total):**
 * - Each body can belong to MULTIPLE groups (bitfield)
 * - Used for more complex filtering scenarios
 * 
 * **Priority:**
 * 1. Custom filter callback (if set)
 * 2. Layer mask (broad-phase)
 * 3. Group filter (narrow-phase)
 * 4. Default (collide)
 */
struct CollisionLayers {
    // ... existing fields from #009 ...
    
    /// Collision groups (bitfield)
    uint16_t groups = 0xFFFF;  ///< Groups this body belongs to (all by default)
    
    /// Group mask (which groups to collide with)
    uint16_t groupMask = 0xFFFF;  ///< Which groups to collide with (all by default)
    
    /**
     * @brief Check if collides with another body (layer + group)
     * @param other Other collision layers
     * @return true if collision enabled
     */
    bool ShouldCollideWith(const CollisionLayers &other) const {
        // Check layer mask
        if ((mask & (1u << other.layer)) == 0) return false;
        
        // Check group intersection
        if ((groups & other.groupMask) == 0 && (groupMask & other.groups) == 0) {
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Set collision group
     * @param group Group index [0, 15]
     */
    void AddToGroup(uint16_t group) {
        groups |= (1u << group);
    }
    
    /**
     * @brief Remove from collision group
     * @param group Group index [0, 15]
     */
    void RemoveFromGroup(uint16_t group) {
        groups &= ~(1u << group);
    }
    
    /**
     * @brief Check if in group
     * @param group Group index
     * @return true if in group
     */
    bool IsInGroup(uint16_t group) const {
        return (groups & (1u << group)) != 0;
    }
};

} // namespace Physics::Component
```

### CollisionFilter Component

```cpp
/**************************************************************************
 * @file CollisionFilter.hpp
 * @brief Custom collision filter callbacks
 **************************************************************************/

#pragma once

#include <functional>
#include <cstdint>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Custom collision filter callback
 * 
 * Allows game-specific collision logic beyond layers/groups.
 * 
 * **Use cases:**
 * - One-way platforms (player can jump through from below)
 * - Invincibility frames (character can't be hit temporarily)
 * - Friendly fire toggle
 * - Phasing through specific entities
 * - Distance-based collision
 * 
 * **Callback signature:**
 * - Parameters: (this entity, other entity, Core)
 * - Return: true to collide, false to ignore
 * 
 * **Performance:**
 * - Called AFTER layer/group filtering (narrow-phase only)
 * - Keep logic fast (< 0.01ms)
 * - Avoid expensive queries
 * 
 * @note Callback is checked EVERY potential collision
 * @note Use layers/groups for broad categories, callbacks for special cases
 */
struct CollisionFilter {
    using FilterCallback = std::function<bool(Engine::Entity self,
                                              Engine::Entity other,
                                              Engine::Core &core)>;
    
    FilterCallback callback = nullptr;  ///< Custom filter function
    
    /**
     * @brief Set filter callback
     * @param cb Filter function
     */
    void SetCallback(FilterCallback cb) {
        callback = std::move(cb);
    }
    
    /**
     * @brief Check if should collide (invoke callback)
     * @param self This entity
     * @param other Other entity
     * @param core Engine core
     * @return true if collision allowed
     */
    bool ShouldCollide(Engine::Entity self, Engine::Entity other,
                       Engine::Core &core) const {
        if (callback) {
            return callback(self, other, core);
        }
        return true;  // Default: collide
    }
    
    /**
     * @brief Check if callback is set
     * @return true if custom filter active
     */
    bool HasCallback() const {
        return callback != nullptr;
    }
    
    /**
     * @brief Clear callback
     */
    void ClearCallback() {
        callback = nullptr;
    }
};

} // namespace Physics::Component
```

### CollisionMatrix Resource

```cpp
/**************************************************************************
 * @file CollisionMatrix.hpp
 * @brief Global collision layer matrix (which layers collide)
 **************************************************************************/

#pragma once

#include <cstdint>
#include <array>

namespace Physics::Resource {

/**
 * @brief Global collision matrix
 * 
 * Defines which layers can collide with which other layers.
 * Used for broad-phase filtering before expensive collision checks.
 * 
 * **Structure:**
 * - 32x32 bit matrix
 * - matrix[layerA][layerB] = can collide?
 * - Symmetric (if A collides with B, then B collides with A)
 * 
 * **Default Configuration:**
 * - All layers collide with all layers
 * - Customize via SetLayerCollision()
 * 
 * **Performance:**
 * - Very fast (single bit check)
 * - Applied before any shape checks
 * - Reduces collision pairs by 50-90% typically
 */
class CollisionMatrix {
public:
    /**
     * @brief Construct with default (all layers collide)
     */
    CollisionMatrix() {
        // Default: all collide
        for (uint32_t i = 0; i < 32; ++i) {
            layerMasks_[i] = 0xFFFFFFFF;
        }
    }
    
    /**
     * @brief Set whether two layers collide
     * @param layerA First layer [0, 31]
     * @param layerB Second layer [0, 31]
     * @param collide true to enable collision, false to disable
     * 
     * Automatically updates both directions (symmetric).
     * 
     * @example
     * @code
     * CollisionMatrix matrix;
     * // Characters don't collide with each other
     * matrix.SetLayerCollision(
     *     CollisionLayers::CHARACTER,
     *     CollisionLayers::CHARACTER,
     *     false
     * );
     * @endcode
     */
    void SetLayerCollision(uint32_t layerA, uint32_t layerB, bool collide) {
        if (layerA >= 32 || layerB >= 32) return;
        
        if (collide) {
            layerMasks_[layerA] |= (1u << layerB);
            layerMasks_[layerB] |= (1u << layerA);
        } else {
            layerMasks_[layerA] &= ~(1u << layerB);
            layerMasks_[layerB] &= ~(1u << layerA);
        }
    }
    
    /**
     * @brief Check if two layers collide
     * @param layerA First layer
     * @param layerB Second layer
     * @return true if collision enabled
     */
    bool CanLayersCollide(uint32_t layerA, uint32_t layerB) const {
        if (layerA >= 32 || layerB >= 32) return false;
        return (layerMasks_[layerA] & (1u << layerB)) != 0;
    }
    
    /**
     * @brief Get layer mask for specific layer
     * @param layer Layer index
     * @return Bitmask of layers this layer collides with
     */
    uint32_t GetLayerMask(uint32_t layer) const {
        if (layer >= 32) return 0;
        return layerMasks_[layer];
    }
    
    /**
     * @brief Disable all collisions for a layer
     * @param layer Layer to isolate
     */
    void IsolateLayer(uint32_t layer) {
        if (layer >= 32) return;
        
        // Disable collisions with all other layers
        for (uint32_t i = 0; i < 32; ++i) {
            if (i != layer) {
                SetLayerCollision(layer, i, false);
            }
        }
    }
    
    /**
     * @brief Reset to default (all collide)
     */
    void Reset() {
        for (uint32_t i = 0; i < 32; ++i) {
            layerMasks_[i] = 0xFFFFFFFF;
        }
    }
    
    /**
     * @brief Configure common layer setup (FPS game)
     */
    static CollisionMatrix CreateFPSSetup() {
        CollisionMatrix matrix;
        
        // Characters don't collide with each other (prevent pushback)
        matrix.SetLayerCollision(3, 3, false);  // CHARACTER with CHARACTER
        
        // Projectiles only collide with static and characters
        matrix.SetLayerCollision(4, 4, false);  // PROJECTILE with PROJECTILE
        matrix.SetLayerCollision(4, 2, false);  // PROJECTILE with DYNAMIC
        
        // Triggers don't physically collide (only overlap)
        matrix.IsolateLayer(5);  // TRIGGER
        
        return matrix;
    }
    
    /**
     * @brief Configure common layer setup (platformer game)
     */
    static CollisionMatrix CreatePlatformerSetup() {
        CollisionMatrix matrix;
        
        // Player and enemies can collide
        // Pickups don't collide physically (only trigger)
        matrix.IsolateLayer(6);  // PICKUP layer
        
        return matrix;
    }

private:
    std::array<uint32_t, 32> layerMasks_;  ///< Collision masks per layer
};

} // namespace Physics::Resource
```

### CollisionFilterSystem

```cpp
/**************************************************************************
 * @file CollisionFilterSystem.hpp
 * @brief System for applying collision filters
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::Resource {
    class CollisionMatrix;
}

namespace Physics::System {

/**
 * @brief Collision filter system
 * 
 * Manages collision filtering at different stages:
 * 1. Broad-phase: Layer matrix (fastest)
 * 2. Narrow-phase: Group filtering
 * 3. Custom: Callback functions (slowest)
 */
class CollisionFilterSystem {
public:
    /**
     * @brief Initialize collision filter system
     * @param core Engine core
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown collision filter system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Set global collision matrix
     * @param core Engine core
     * @param matrix Collision matrix
     */
    static void SetCollisionMatrix(Engine::Core &core,
                                    const Resource::CollisionMatrix &matrix);
    
    /**
     * @brief Get global collision matrix
     * @param core Engine core
     * @return Current collision matrix
     */
    static Resource::CollisionMatrix& GetCollisionMatrix(Engine::Core &core);
    
    /**
     * @brief Check if two entities should collide (full pipeline)
     * @param core Engine core
     * @param entityA First entity
     * @param entityB Second entity
     * @return true if collision allowed
     * 
     * Applies filters in order:
     * 1. Layer matrix (broad-phase)
     * 2. Group filtering
     * 3. Custom callbacks (if set)
     */
    static bool ShouldCollide(Engine::Core &core,
                              Engine::Entity entityA,
                              Engine::Entity entityB);

private:
    /// Jolt collision filter implementation
    class JoltObjectLayerPairFilter;
    class JoltBroadPhaseLayerPairFilter;
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Extend CollisionLayers (2-3 hours)

- [ ] Update `component/CollisionLayers.hpp` from #009
- [ ] Add groups bitfield (uint16_t)
- [ ] Add groupMask field
- [ ] Implement AddToGroup(), RemoveFromGroup()
- [ ] Implement IsInGroup()
- [ ] Update ShouldCollideWith() to check groups
- [ ] Update Doxygen comments

### Phase 2: Create CollisionFilter Component (2-3 hours)

- [ ] Create `component/CollisionFilter.hpp`
- [ ] Implement CollisionFilter struct
- [ ] Add FilterCallback type
- [ ] Implement SetCallback(), ShouldCollide()
- [ ] Add HasCallback(), ClearCallback()
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 3: Implement CollisionMatrix Resource (4-5 hours)

- [ ] Create `resource/CollisionMatrix.hpp`
- [ ] Implement CollisionMatrix class
- [ ] Add 32x32 bit matrix storage
- [ ] Implement SetLayerCollision()
- [ ] Implement CanLayersCollide()
- [ ] Implement IsolateLayer()
- [ ] Add CreateFPSSetup(), CreatePlatformerSetup()
- [ ] Ensure symmetry (A→B = B→A)

### Phase 4: Implement CollisionFilterSystem (8-10 hours)

- [ ] Create `system/CollisionFilterSystem.hpp`
- [ ] Implement Initialize(), Shutdown()
- [ ] Store global CollisionMatrix
- [ ] Implement ShouldCollide() (full pipeline)
  - Check layer matrix
  - Check group filtering
  - Invoke custom callbacks
- [ ] Integrate with Jolt filters
  - JPH::ObjectLayerPairFilter
  - JPH::BroadPhaseLayerPairFilter
- [ ] Update RigidBodySystem to use filters

### Phase 5: Update Queries with Filtering (3-4 hours)

- [ ] Update RaycastQuery (#009) to use filters
- [ ] Update ShapeCastQuery (#010) to use filters
- [ ] Update OverlapQuery (#011) to use filters
- [ ] Apply layer matrix before Jolt queries
- [ ] Apply custom callbacks in results

### Phase 6: Unit Tests (6-7 hours)

- [ ] Create `tests/CollisionFilterTest.cpp`
- [ ] Test layer collision enable/disable
- [ ] Test group filtering
- [ ] Test custom callback
- [ ] Test filter priority (callback > group > layer)
- [ ] Test symmetric collisions
- [ ] Create `tests/CollisionMatrixTest.cpp`
- [ ] Test matrix operations
- [ ] Test preset configurations

### Phase 7: Integration Examples (5-6 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "FPS Collision Setup" example
  - Characters don't push each other
  - Projectiles ignore each other
- [ ] Add "One-Way Platform" example
  - Custom callback for directional collision
- [ ] Add "Invincibility Frames" example
  - Temporary collision disable
- [ ] Add "Team-Based Collision" example
  - Group-based friendly fire

### Phase 8: Documentation (3-4 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Collision Filtering" section
  - Explain layers vs groups vs callbacks
  - Performance guidelines
- [ ] Add practical examples
- [ ] Document common patterns

## 🧪 Testing Checklist

### Unit Tests

- [ ] Layer collision matrix works
- [ ] SetLayerCollision is symmetric
- [ ] Group filtering works
- [ ] Custom callback invoked
- [ ] Filter priority correct
- [ ] IsolateLayer() works
- [ ] Preset configs work
- [ ] ShouldCollide() full pipeline
- [ ] Queries respect filters

### Integration Tests

- [ ] FPS setup prevents character-character collision
- [ ] One-way platforms work
- [ ] Invincibility frames work
- [ ] Team collision works
- [ ] Projectiles filtered correctly

### Performance Tests

- [ ] Layer matrix check: < 0.001ms
- [ ] Group check: < 0.001ms
- [ ] Custom callback: < 0.01ms
- [ ] 1000 filter checks: < 5ms
- [ ] Broad-phase rejection: 50%+ reduction

### Edge Cases

- [ ] Invalid layer indices
- [ ] Null callbacks
- [ ] Empty groups
- [ ] Self-collision

## 📚 Code Examples

### Example 1: FPS Game Collision Setup

```cpp
// Setup collision matrix for FPS game
void SetupFPSCollisions(Engine::Core &core) {
    auto matrix = Physics::Resource::CollisionMatrix::CreateFPSSetup();
    
    // Additional customization
    // Projectiles should hit static and characters, but not dynamic props
    matrix.SetLayerCollision(
        Physics::Component::CollisionLayers::PROJECTILE,
        Physics::Component::CollisionLayers::DYNAMIC,
        false
    );
    
    // Apply globally
    Physics::System::CollisionFilterSystem::SetCollisionMatrix(core, matrix);
}

// Create entities with correct layers
void CreatePlayer(Engine::Core &core) {
    auto player = core.CreateEntity();
    
    // ... transform, mesh, etc ...
    
    // Physics
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(80.0f);
    player.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Collision layers
    Physics::Component::CollisionLayers layers = Physics::Component::CollisionLayers::Character();
    player.AddComponent<Physics::Component::CollisionLayers>(core, layers);
}
```

### Example 2: One-Way Platform (Custom Callback)

```cpp
// Platform that player can jump through from below
void CreateOneWayPlatform(Engine::Core &core, glm::vec3 position) {
    auto platform = core.CreateEntity();
    
    // Transform
    platform.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    // Physics: static body
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
    platform.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Box collider
    Physics::Component::BoxCollider collider(glm::vec3(2.0f, 0.1f, 2.0f));
    platform.AddComponent<Physics::Component::BoxCollider>(core, collider);
    
    // Custom collision filter: only collide if player is above
    Physics::Component::CollisionFilter filter;
    filter.SetCallback([platform](Engine::Entity self, Engine::Entity other,
                                  Engine::Core &core) -> bool {
        // Get transforms
        auto &platformTransform = self.GetComponent<Object::Component::Transform>(core);
        auto &otherTransform = other.GetComponent<Object::Component::Transform>(core);
        
        // Only collide if other entity is above platform
        return otherTransform.position.y > platformTransform.position.y;
    });
    platform.AddComponent<Physics::Component::CollisionFilter>(core, filter);
}
```

### Example 3: Invincibility Frames (Temporary Collision Disable)

```cpp
// Make character invincible for duration
void MakeInvincible(Engine::Core &core, Engine::Entity character, float duration) {
    // Add custom filter that rejects damage-dealing entities
    Physics::Component::CollisionFilter filter;
    
    auto startTime = std::chrono::steady_clock::now();
    filter.SetCallback([startTime, duration](Engine::Entity self, Engine::Entity other,
                                              Engine::Core &core) -> bool {
        // Check if invincibility expired
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<float>(now - startTime).count();
        
        if (elapsed > duration) {
            // Expired - collide normally
            return true;
        }
        
        // Check if other is damage-dealing
        if (other.HasComponent<Game::DamageOnContact>(core)) {
            return false;  // Ignore damage sources
        }
        
        return true;  // Collide with non-damaging objects
    });
    
    character.AddComponent<Physics::Component::CollisionFilter>(core, filter);
    
    // Remove filter after duration
    // (in real implementation, use timer system)
}
```

### Example 4: Team-Based Collision (Group Filtering)

```cpp
// Teams use collision groups
enum Teams : uint16_t {
    TEAM_RED = 0,
    TEAM_BLUE = 1,
    NEUTRAL = 2
};

// Create team member
void CreateTeamMember(Engine::Core &core, Teams team, glm::vec3 position) {
    auto entity = core.CreateEntity();
    
    // ... transform, mesh, etc ...
    
    // Physics
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(70.0f);
    entity.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Collision layers with team group
    Physics::Component::CollisionLayers layers = Physics::Component::CollisionLayers::Character();
    
    // Add to team group
    layers.AddToGroup(team);
    
    // Friendly fire OFF: don't collide with own team
    layers.groupMask = 0xFFFF & ~(1u << team);
    
    entity.AddComponent<Physics::Component::CollisionLayers>(core, layers);
}

// Projectile that damages only enemy team
void CreateProjectile(Engine::Core &core, Teams ownerTeam, glm::vec3 position, glm::vec3 velocity) {
    auto projectile = core.CreateEntity();
    
    // ... setup ...
    
    Physics::Component::CollisionLayers layers = Physics::Component::CollisionLayers::Projectile();
    
    // Belong to owner's team
    layers.AddToGroup(ownerTeam);
    
    // Only damage enemy teams (mask excludes own team)
    layers.groupMask = 0xFFFF & ~(1u << ownerTeam);
    
    projectile.AddComponent<Physics::Component::CollisionLayers>(core, layers);
}
```

### Example 5: Raycast with Layer Filtering

```cpp
// Raycast that ignores specific layers
void ShootWeapon(Engine::Core &core, Engine::Entity shooter) {
    auto &transform = shooter.GetComponent<Object::Component::Transform>(core);
    
    // Build layer mask: hit everything except triggers and projectiles
    uint32_t layerMask = 0xFFFFFFFF;
    layerMask &= ~(1u << Physics::Component::CollisionLayers::TRIGGER);
    layerMask &= ~(1u << Physics::Component::CollisionLayers::PROJECTILE);
    
    // Raycast with filter
    auto hit = Physics::Resource::RaycastQuery::CastRay(
        core,
        transform.position,
        transform.GetForward(),
        100.0f,
        layerMask  // Apply layer filter
    );
    
    if (hit) {
        Logger::Info("Hit entity on layer: {}",
                     hit.entity.GetComponent<Physics::Component::CollisionLayers>(core).layer);
    }
}
```

### Example 6: Dynamic Filter Toggling

```cpp
// Toggle collision with specific entity dynamically
void ToggleCollisionWith(Engine::Core &core, Engine::Entity entity,
                         Engine::Entity target, bool enable) {
    // Get or add collision filter
    Physics::Component::CollisionFilter filter;
    if (entity.HasComponent<Physics::Component::CollisionFilter>(core)) {
        filter = entity.GetComponent<Physics::Component::CollisionFilter>(core);
    }
    
    // Set callback to ignore target
    filter.SetCallback([target, enable](Engine::Entity self, Engine::Entity other,
                                         Engine::Core &core) -> bool {
        if (other == target) {
            return enable;  // Enable or disable collision with target
        }
        return true;  // Collide with everyone else
    });
    
    entity.AddOrReplaceComponent<Physics::Component::CollisionFilter>(core, filter);
}

// Example usage: Ghost through door
void OpenDoor(Engine::Core &core, Engine::Entity player, Engine::Entity door) {
    // Disable collision between player and door
    ToggleCollisionWith(core, player, door, false);
    
    // Re-enable after 2 seconds (player passed through)
    // (use timer system in real implementation)
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid layer indices**
   ```cpp
   if (layer >= 32) {
       Logger::Error("CollisionFilter: Invalid layer index ({})", layer);
       return;
   }
   ```

2. **Invalid group indices**
   ```cpp
   if (group >= 16) {
       Logger::Error("CollisionFilter: Invalid group index ({})", group);
       return;
   }
   ```

3. **Callback exceptions**
   ```cpp
   try {
       return callback(self, other, core);
   } catch (const std::exception &e) {
       Logger::Error("CollisionFilter: Callback threw exception: {}", e.what());
       return true;  // Default: collide
   }
   ```

4. **Missing components**
   ```cpp
   if (!entityA.HasComponent<CollisionLayers>(core)) {
       Logger::Warn("CollisionFilter: Entity missing CollisionLayers (using default)");
       // Use default layers
   }
   ```

## 💡 Design Considerations

### Filter Priority

```
1. Custom Callback (slowest, most flexible)
   ↓ (if passes or not set)
2. Group Filtering (medium speed)
   ↓ (if passes)
3. Layer Matrix (fastest, broad-phase)
   ↓ (if passes)
4. Collision Check (Jolt)
```

**Performance tip:** Use layers for broad categories, groups for teams/factions, callbacks for special cases.

### Layers vs Groups vs Callbacks

| Method | Speed | Flexibility | Use Case |
|--------|-------|-------------|----------|
| **Layers** | Fastest | Low | Broad categories (static, dynamic, character) |
| **Groups** | Fast | Medium | Teams, factions, affiliation |
| **Callbacks** | Slow | High | Special logic (one-way, temporary, distance-based) |

### Symmetric vs Asymmetric Filtering

**Symmetric (recommended):**
```cpp
// If A ignores B, then B ignores A
matrix.SetLayerCollision(layerA, layerB, false);
// Automatically updates both directions
```

**Asymmetric (advanced, rare):**
```cpp
// A ignores B, but B can hit A (not supported by default)
// Requires custom callback
```

### Callback Performance

```cpp
// ✅ FAST: Simple checks
filter.SetCallback([](Entity self, Entity other, Core &core) {
    return other.HasComponent<EnemyTag>(core);  // Component check
});

// ⚠️ MEDIUM: Component access
filter.SetCallback([](Entity self, Entity other, Core &core) {
    auto &health = other.GetComponent<Health>(core);
    return health.IsAlive();  // Read component data
});

// ❌ SLOW: Complex queries (avoid!)
filter.SetCallback([](Entity self, Entity other, Core &core) {
    auto hits = RaycastQuery::CastRayAll(/* ... */);  // Nested query!
    // ... expensive logic ...
    return true;
});
```

## 📊 Success Criteria

- [ ] CollisionLayers extended with groups
- [ ] CollisionFilter component implemented
- [ ] CollisionMatrix resource implemented
- [ ] CollisionFilterSystem implemented
- [ ] Layer matrix filtering works
- [ ] Group filtering works
- [ ] Custom callbacks work
- [ ] Filter priority correct
- [ ] Queries respect filters
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 0.01ms per filter)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Collision Filtering: https://jrouwe.github.io/JoltPhysics/
- Unity Collision Matrix: https://docs.unity3d.com/Manual/LayerBasedCollision.html
- Godot Collision Layers: https://docs.godotengine.org/en/stable/tutorials/physics/physics_introduction.html#collision-layers-and-masks
- Unreal Collision Filtering: https://docs.unrealengine.com/5.0/en-US/collision-filtering-in-unreal-engine/

## 🚀 Follow-up Issues

- #013: Trigger Volumes (overlap-based events)
- #014: Contact Modification (friction/restitution override)
- #020: Debug Visualization (show collision layers)
- #024: Performance Profiling (filter statistics)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.5`, `priority:high`, `filtering`  
**Milestone:** v0.5 - Collision Queries
