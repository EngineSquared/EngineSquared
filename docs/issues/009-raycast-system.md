# Issue #009: Implement Raycast System

**Milestone:** v0.5 - Collision Queries  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 3-4 days  
**Dependencies:** #001-008 (Core physics)  
**Related Issues:** #010 (ShapeCast), #011 (Overlap), #012 (Filtering)

---

## 📋 Description

Implement comprehensive raycasting system for collision detection queries. Essential for line-of-sight checks, weapon projectiles, ground detection, object picking, and AI perception.

Raycasts are one of the most frequently used physics queries in games and require both accuracy and performance optimization.

## 🎯 Goals

- ✅ Implement Raycast API (single hit)
- ✅ Implement RaycastAll API (multiple hits)
- ✅ Support layer-based filtering
- ✅ Provide detailed hit information
- ✅ Optimize for common use cases
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── RaycastQuery.hpp         # New file
│   └── RaycastQuery.cpp         # New file
├── component/
│   ├── RaycastHit.hpp          # New file (event/result)
│   └── CollisionLayers.hpp     # New file (filtering)
├── system/
│   └── PhysicsWorldSystem.cpp  # Update with query support
└── tests/
    ├── RaycastTest.cpp         # New file
    └── CollisionLayersTest.cpp # New file
```

### RaycastHit Result

```cpp
/**************************************************************************
 * @file RaycastHit.hpp
 * @brief Raycast hit result information
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <optional>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Raycast hit information
 * 
 * Contains all information about a raycast collision:
 * - Hit entity
 * - Hit point in world space
 * - Surface normal
 * - Distance from ray origin
 * - Barycentric coordinates (for triangle hits)
 */
struct RaycastHit {
    bool hit = false;                      ///< Whether ray hit anything
    Engine::Entity entity;                 ///< Hit entity (invalid if no hit)
    glm::vec3 point = glm::vec3(0.0f);    ///< Hit point in world space
    glm::vec3 normal = glm::vec3(0.0f);   ///< Surface normal at hit point
    float distance = 0.0f;                 ///< Distance from ray origin to hit
    float fraction = 0.0f;                 ///< Hit position along ray [0, 1]

    /**
     * @brief Check if raycast hit something
     * @return true if hit, false if miss
     */
    operator bool() const { return hit; }

    /**
     * @brief Get hit entity (if valid)
     * @return Optional entity
     */
    std::optional<Engine::Entity> GetEntity() const {
        return hit ? std::optional(entity) : std::nullopt;
    }
};

} // namespace Physics::Component
```

### CollisionLayers Component

```cpp
/**************************************************************************
 * @file CollisionLayers.hpp
 * @brief Collision layer filtering system
 **************************************************************************/

#pragma once

#include <cstdint>

namespace Physics::Component {

/**
 * @brief Collision layers for filtering interactions
 * 
 * Uses bitmasking for efficient layer checks.
 * Each body belongs to ONE layer and can collide with multiple layers (mask).
 * 
 * **Default Layers:**
 * - Layer 0: Default
 * - Layer 1: Static (terrain, buildings)
 * - Layer 2: Dynamic (props)
 * - Layer 3: Character
 * - Layer 4: Projectile
 * - Layer 5: Trigger
 * - Layer 6-31: Custom
 * 
 * @example
 * @code
 * // Character collides with everything except other characters
 * CollisionLayers layers;
 * layers.layer = 3;  // Character layer
 * layers.mask = ~(1u << 3);  // Collide with all except layer 3
 * @endcode
 */
struct CollisionLayers {
    uint32_t layer = 0;           ///< Object layer (0-31)
    uint32_t mask = 0xFFFFFFFF;   ///< Collision mask (which layers to collide with)

    /// Default layers
    static constexpr uint32_t DEFAULT = 0;
    static constexpr uint32_t STATIC = 1;
    static constexpr uint32_t DYNAMIC = 2;
    static constexpr uint32_t CHARACTER = 3;
    static constexpr uint32_t PROJECTILE = 4;
    static constexpr uint32_t TRIGGER = 5;

    /**
     * @brief Create default collision layers
     * @return CollisionLayers (layer 0, collides with all)
     */
    static CollisionLayers Default() {
        return {DEFAULT, 0xFFFFFFFF};
    }

    /**
     * @brief Create static object layers
     * @return CollisionLayers (layer 1, collides with dynamic/characters/projectiles)
     */
    static CollisionLayers Static() {
        return {STATIC, (1u << DYNAMIC) | (1u << CHARACTER) | (1u << PROJECTILE)};
    }

    /**
     * @brief Create dynamic object layers
     * @return CollisionLayers (layer 2, collides with all)
     */
    static CollisionLayers Dynamic() {
        return {DYNAMIC, 0xFFFFFFFF};
    }

    /**
     * @brief Create character layers
     * @return CollisionLayers (layer 3, collides with static/dynamic/projectiles)
     */
    static CollisionLayers Character() {
        return {CHARACTER, (1u << STATIC) | (1u << DYNAMIC) | (1u << PROJECTILE)};
    }

    /**
     * @brief Create projectile layers
     * @return CollisionLayers (layer 4, collides with static/dynamic/characters)
     */
    static CollisionLayers Projectile() {
        return {PROJECTILE, (1u << STATIC) | (1u << DYNAMIC) | (1u << CHARACTER)};
    }

    /**
     * @brief Create trigger layers
     * @return CollisionLayers (layer 5, collides with characters only)
     */
    static CollisionLayers Trigger() {
        return {TRIGGER, (1u << CHARACTER)};
    }

    /**
     * @brief Check if this layer collides with another
     * @param other Other layer
     * @return true if collision enabled
     */
    bool CollidesWith(const CollisionLayers &other) const {
        return (mask & (1u << other.layer)) != 0;
    }

    /**
     * @brief Set layer
     * @param l Layer index [0, 31]
     */
    void SetLayer(uint32_t l) {
        layer = l & 0x1F;  // Clamp to [0, 31]
    }

    /**
     * @brief Enable collision with layer
     * @param l Layer index
     */
    void EnableLayer(uint32_t l) {
        mask |= (1u << l);
    }

    /**
     * @brief Disable collision with layer
     * @param l Layer index
     */
    void DisableLayer(uint32_t l) {
        mask &= ~(1u << l);
    }
};

} // namespace Physics::Component
```

### RaycastQuery Resource

```cpp
/**************************************************************************
 * @file RaycastQuery.hpp
 * @brief Raycast query functions (collision detection queries)
 **************************************************************************/

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {
    struct RaycastHit;
    struct CollisionLayers;
}

namespace Physics::Resource {

/**
 * @brief Raycast query utilities
 * 
 * Provides functions for performing raycasts against the physics world.
 * 
 * **Responsibilities:**
 * - Single raycast (closest hit)
 * - Multi-raycast (all hits)
 * - Layer filtering
 * - Distance limiting
 */
class RaycastQuery {
public:
    /**
     * @brief Cast ray and return closest hit
     * @param core Engine core
     * @param origin Ray origin in world space
     * @param direction Ray direction (will be normalized)
     * @param maxDistance Maximum ray distance
     * @param layerMask Collision layer mask (default: all layers)
     * @return RaycastHit result (hit=false if no collision)
     * 
     * @example
     * @code
     * // Check if player can see enemy
     * auto hit = Physics::Resource::RaycastQuery::CastRay(
     *     core,
     *     playerPos,
     *     glm::normalize(enemyPos - playerPos),
     *     100.0f
     * );
     * if (hit && hit.entity == enemy) {
     *     // Line of sight clear
     * }
     * @endcode
     */
    static Component::RaycastHit CastRay(
        Engine::Core &core,
        const glm::vec3 &origin,
        const glm::vec3 &direction,
        float maxDistance = 1000.0f,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Cast ray and return all hits (sorted by distance)
     * @param core Engine core
     * @param origin Ray origin in world space
     * @param direction Ray direction (will be normalized)
     * @param maxDistance Maximum ray distance
     * @param layerMask Collision layer mask (default: all layers)
     * @return Vector of RaycastHit (empty if no collisions)
     * 
     * @example
     * @code
     * // Find all objects along ray (for penetrating projectile)
     * auto hits = Physics::Resource::RaycastQuery::CastRayAll(
     *     core, origin, direction, 50.0f
     * );
     * for (const auto &hit : hits) {
     *     // Damage each object hit
     * }
     * @endcode
     */
    static std::vector<Component::RaycastHit> CastRayAll(
        Engine::Core &core,
        const glm::vec3 &origin,
        const glm::vec3 &direction,
        float maxDistance = 1000.0f,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Cast ray between two points
     * @param core Engine core
     * @param start Start point
     * @param end End point
     * @param layerMask Collision layer mask
     * @return RaycastHit result
     * 
     * Convenience method for point-to-point raycasts.
     */
    static Component::RaycastHit CastRayBetween(
        Engine::Core &core,
        const glm::vec3 &start,
        const glm::vec3 &end,
        uint32_t layerMask = 0xFFFFFFFF
    ) {
        glm::vec3 dir = end - start;
        float distance = glm::length(dir);
        return CastRay(core, start, dir / distance, distance, layerMask);
    }

    /**
     * @brief Cast ray and ignore specific entity
     * @param core Engine core
     * @param origin Ray origin
     * @param direction Ray direction
     * @param maxDistance Maximum distance
     * @param ignoreEntity Entity to ignore (e.g., player firing raycast)
     * @param layerMask Collision layer mask
     * @return RaycastHit result
     * 
     * Useful for weapon raycasts that should ignore the firing entity.
     */
    static Component::RaycastHit CastRayIgnore(
        Engine::Core &core,
        const glm::vec3 &origin,
        const glm::vec3 &direction,
        float maxDistance,
        Engine::Entity ignoreEntity,
        uint32_t layerMask = 0xFFFFFFFF
    );

private:
    /// Convert Jolt raycast result to RaycastHit
    static Component::RaycastHit ConvertJoltHit(const void *joltHit, Engine::Core &core);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create RaycastHit & CollisionLayers (2-3 hours)

- [ ] Create `component/RaycastHit.hpp`
- [ ] Implement RaycastHit struct
- [ ] Add hit fields (entity, point, normal, distance, fraction)
- [ ] Add convenience methods
- [ ] Create `component/CollisionLayers.hpp`
- [ ] Implement CollisionLayers struct
- [ ] Add default layer constants
- [ ] Add layer manipulation methods
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement RaycastQuery Resource (6-8 hours)

- [ ] Create `resource/RaycastQuery.hpp`
- [ ] Implement CastRay() (single hit)
  - Create JPH::RRayCast
  - Call PhysicsSystem.CastRay()
  - Convert result to RaycastHit
- [ ] Implement CastRayAll() (multiple hits)
  - Use JPH::AllHitCollisionCollector
  - Sort hits by distance
- [ ] Implement CastRayBetween()
- [ ] Implement CastRayIgnore()
- [ ] Add layer filtering support
- [ ] Handle invalid inputs

### Phase 3: Integrate with PhysicsWorldSystem (4-5 hours)

- [ ] Update `system/PhysicsWorldSystem.cpp`
- [ ] Store JPH::PhysicsSystem reference
- [ ] Implement CastRay wrapper
  - Create JPH::RRayCast
  - Use JPH::CastRayCollector
  - Map body ID back to entity
- [ ] Add layer filter to Jolt queries
- [ ] Handle edge cases (zero direction, negative distance)

### Phase 4: Add CollisionLayers to RigidBody (2-3 hours)

- [ ] Update `component/RigidBody.hpp`
- [ ] Add `CollisionLayers layers` field
- [ ] Update RigidBodySystem to apply layers
- [ ] Set Jolt ObjectLayer and BroadPhaseLayer
- [ ] Update factory methods

### Phase 5: Unit Tests (5-6 hours)

- [ ] Create `tests/RaycastTest.cpp`
- [ ] Test single raycast hit
- [ ] Test raycast miss
- [ ] Test raycast multiple hits
- [ ] Test distance limiting
- [ ] Test layer filtering
- [ ] Test ignore entity
- [ ] Test edge cases (zero direction, behind origin)
- [ ] Create `tests/CollisionLayersTest.cpp`
- [ ] Test layer collision logic
- [ ] Test default layers

### Phase 6: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Ground Detection" example
  - Raycast downward from character
- [ ] Add "Line of Sight" example
  - Check visibility between entities
- [ ] Add "Object Picking" example
  - Raycast from camera to mouse position
- [ ] Add "Projectile Simulation" example
  - Hitscan weapon with raycast

### Phase 7: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Raycasting" section
  - Explain layer system
  - Performance tips
- [ ] Add usage examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Raycast hits static object
- [ ] Raycast hits dynamic object
- [ ] Raycast misses (no hit)
- [ ] Raycast returns correct hit point
- [ ] Raycast returns correct normal
- [ ] Raycast returns correct distance
- [ ] RaycastAll returns multiple hits (sorted)
- [ ] Distance limiting works
- [ ] Layer filtering works
- [ ] Ignore entity works
- [ ] Zero direction is rejected
- [ ] Negative distance is rejected

### Integration Tests

- [ ] Ground detection works
- [ ] Line of sight detection accurate
- [ ] Object picking works with camera
- [ ] Raycast penetrates multiple objects

### Performance Tests

- [ ] Single raycast: < 0.1ms
- [ ] 100 raycasts: < 5ms
- [ ] 1000 raycasts: < 50ms
- [ ] Layer filtering reduces overhead

### Edge Cases

- [ ] Raycast from inside object
- [ ] Raycast along surface (grazing)
- [ ] Very long rays (10000+ units)
- [ ] Raycast at world boundaries

## 📚 Code Examples

### Example 1: Ground Detection (Character Controller)

```cpp
// Check if character is on ground
glm::vec3 characterPos = transform.position;
glm::vec3 down = glm::vec3(0, -1, 0);
float groundCheckDistance = 0.2f;

auto hit = Physics::Resource::RaycastQuery::CastRay(
    core,
    characterPos,
    down,
    groundCheckDistance,
    (1u << Physics::Component::CollisionLayers::STATIC)  // Only check static
);

if (hit) {
    // On ground
    canJump = true;
    Logger::Info("Ground distance: {}", hit.distance);
} else {
    // In air
    canJump = false;
}
```

### Example 2: Line of Sight Check (AI)

```cpp
// Check if enemy can see player
auto enemyTransform = enemy.GetComponent<Object::Component::Transform>(core);
auto playerTransform = player.GetComponent<Object::Component::Transform>(core);

glm::vec3 toPlayer = playerTransform.position - enemyTransform.position;
float distance = glm::length(toPlayer);

// Cast ray from enemy to player
auto hit = Physics::Resource::RaycastQuery::CastRay(
    core,
    enemyTransform.position,
    glm::normalize(toPlayer),
    distance,
    0xFFFFFFFF  // Check all layers
);

if (hit && hit.entity == player) {
    // Direct line of sight - no obstacles
    enemyAI.StartChasing();
} else if (hit) {
    // Something blocking (wall, cover, etc.)
    enemyAI.Investigate(hit.point);
} else {
    // Player too far or out of range
    enemyAI.Patrol();
}
```

### Example 3: Hitscan Weapon

```cpp
// Instant-hit weapon (no projectile)
void FireWeapon(Engine::Core &core, Engine::Entity shooter) {
    auto transform = shooter.GetComponent<Object::Component::Transform>(core);
    
    glm::vec3 muzzlePos = transform.position + transform.GetForward() * 0.5f;
    glm::vec3 shootDir = transform.GetForward();
    float range = 100.0f;
    
    // Raycast, ignore shooter
    auto hit = Physics::Resource::RaycastQuery::CastRayIgnore(
        core, muzzlePos, shootDir, range, shooter
    );
    
    if (hit) {
        // Hit something
        Logger::Info("Hit at distance: {}", hit.distance);
        
        // Apply damage if hit entity has Health component
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            health.TakeDamage(25.0f);
        }
        
        // Spawn impact effect at hit point
        SpawnImpactEffect(core, hit.point, hit.normal);
    }
}
```

### Example 4: Mouse Object Picking (Editor)

```cpp
// Pick object under mouse cursor
Engine::Entity PickObjectUnderMouse(Engine::Core &core, const Camera &camera,
                                    glm::vec2 mousePos, glm::vec2 screenSize) {
    // Convert mouse to ray
    glm::vec3 rayOrigin = camera.position;
    glm::vec3 rayDir = camera.ScreenToWorldRay(mousePos, screenSize);
    
    // Cast ray from camera
    auto hit = Physics::Resource::RaycastQuery::CastRay(
        core,
        rayOrigin,
        rayDir,
        1000.0f,
        0xFFFFFFFF  // All layers
    );
    
    if (hit) {
        Logger::Info("Picked entity: {}", entt::to_integral(hit.entity));
        return hit.entity;
    }
    
    return Engine::Entity();  // Invalid
}
```

### Example 5: Penetrating Raycast (Bullet Penetration)

```cpp
// Bullet that penetrates thin objects
void FirePenetratingBullet(Engine::Core &core, glm::vec3 origin, glm::vec3 direction) {
    // Get all hits along ray
    auto hits = Physics::Resource::RaycastQuery::CastRayAll(
        core, origin, direction, 50.0f
    );
    
    float penetrationDepth = 0.0f;
    const float maxPenetration = 2.0f;  // Can penetrate 2 units total
    
    for (const auto &hit : hits) {
        // Calculate object thickness (approximate)
        float thickness = 0.5f;  // Assume 0.5 units
        
        if (penetrationDepth + thickness > maxPenetration) {
            // Bullet stopped
            Logger::Info("Bullet stopped at distance: {}", hit.distance);
            SpawnImpactEffect(core, hit.point, hit.normal);
            break;
        }
        
        // Penetrate and damage
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            float damage = 25.0f * (1.0f - penetrationDepth / maxPenetration);
            health.TakeDamage(damage);
        }
        
        penetrationDepth += thickness;
        
        Logger::Info("Penetrated object at distance: {}", hit.distance);
    }
}
```

### Example 6: Layer-Filtered Raycast

```cpp
// Character raycast that ignores triggers
auto &characterLayers = character.GetComponent<Physics::Component::CollisionLayers>(core);

// Build mask: collide with everything except triggers
uint32_t mask = 0xFFFFFFFF & ~(1u << Physics::Component::CollisionLayers::TRIGGER);

auto hit = Physics::Resource::RaycastQuery::CastRay(
    core,
    characterPos,
    direction,
    10.0f,
    mask  // Ignore trigger layer
);
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid direction (zero length)**
   ```cpp
   if (glm::length(direction) < 0.001f) {
       Logger::Error("RaycastQuery: Invalid direction (zero length)");
       return RaycastHit{};  // Miss
   }
   ```

2. **Negative or zero distance**
   ```cpp
   if (maxDistance <= 0.0f) {
       Logger::Error("RaycastQuery: Invalid distance ({})", maxDistance);
       return RaycastHit{};
   }
   ```

3. **Invalid entity in ignore list**
   ```cpp
   if (!ignoreEntity.IsValid(core)) {
       Logger::Warn("RaycastQuery: Ignore entity is invalid");
       // Continue without ignoring
   }
   ```

4. **Physics world not initialized**
   ```cpp
   if (!physicsSystem || !physicsSystem->IsInitialized()) {
       Logger::Error("RaycastQuery: Physics world not initialized");
       return RaycastHit{};
   }
   ```

## 💡 Design Considerations

### Single Hit vs All Hits

**CastRay (single):**
- Faster (stops at first hit)
- Lower memory (one result)
- Most common use case

**CastRayAll (multiple):**
- Slower (collects all hits)
- Higher memory (vector allocation)
- Needed for penetration, debug visualization

**Rule of thumb:** Use `CastRay` unless you specifically need all hits.

### Layer Filtering Performance

```cpp
// ✅ FAST: Filter by layer mask (GPU-friendly)
auto hit = RaycastQuery::CastRay(core, origin, dir, 100.0f,
                                 1u << CollisionLayers::STATIC);

// ❌ SLOW: Check all objects then filter (CPU-bound)
auto hits = RaycastQuery::CastRayAll(core, origin, dir, 100.0f);
for (const auto &hit : hits) {
    if (hit.entity.HasComponent<StaticTag>(core)) {
        // Use hit
    }
}
```

### Raycast from Inside Object

When ray origin is inside a collider:
- Jolt returns hit immediately (distance = 0)
- Consider moving origin slightly outside
- Or use shape cast (issue #010)

### Backface Culling

Jolt supports backface culling (ignore hits from inside):
```cpp
// Future enhancement: add parameter to RaycastQuery
bool ignoreBackfaces = true;
```

## 📊 Success Criteria

- [ ] RaycastHit component implemented
- [ ] CollisionLayers component implemented
- [ ] RaycastQuery resource implemented
- [ ] CastRay() works correctly
- [ ] CastRayAll() works correctly
- [ ] Layer filtering functional
- [ ] Ignore entity works
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 0.1ms single ray)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Raycasting: https://jrouwe.github.io/JoltPhysics/
- Unity Raycast: https://docs.unity3d.com/ScriptReference/Physics.Raycast.html
- Godot Raycast: https://docs.godotengine.org/en/stable/classes/class_physicsrayqueryparameters3d.html
- Unreal Line Trace: https://docs.unrealengine.com/5.0/en-US/BlueprintAPI/Collision/LineTraceByChannel/

## 🚀 Follow-up Issues

- #010: ShapeCast (swept shape queries)
- #011: Overlap Queries (spatial queries)
- #012: Advanced Collision Filtering (groups, callbacks)
- #020: Debug Visualization (draw raycasts in editor)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.5`, `priority:high`, `queries`  
**Milestone:** v0.5 - Collision Queries
