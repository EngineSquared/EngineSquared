# Issue #011: Implement Overlap Queries

**Milestone:** v0.5 - Collision Queries  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 2-3 days  
**Dependencies:** #009 (Raycast), #012 (Filtering)  
**Related Issues:** #010 (ShapeCast)

---

## 📋 Description

Implement spatial overlap queries for detecting all bodies within a region. Essential for area-of-effect detection, proximity queries, trigger volumes, and spatial awareness systems.

Overlap queries provide instant detection of all entities in a volume without requiring movement (unlike shape casts).

## 🎯 Goals

- ✅ Implement OverlapBox API
- ✅ Implement OverlapSphere API
- ✅ Implement OverlapCapsule API
- ✅ Support layer-based filtering
- ✅ Efficient broad-phase queries
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── OverlapQuery.hpp        # New file
│   └── OverlapQuery.cpp        # New file
├── component/
│   └── OverlapResult.hpp       # New file (result data)
└── tests/
    └── OverlapQueryTest.cpp    # New file
```

### OverlapResult Component

```cpp
/**************************************************************************
 * @file OverlapResult.hpp
 * @brief Overlap query result information
 **************************************************************************/

#pragma once

#include <vector>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Single overlap result
 * 
 * Contains entity that overlaps query volume.
 */
struct OverlapHit {
    Engine::Entity entity;  ///< Overlapping entity

    /**
     * @brief Check if valid
     * @return true if entity is valid
     */
    bool IsValid() const;
};

/**
 * @brief Overlap query result collection
 * 
 * Contains all entities that overlap the query volume.
 */
struct OverlapResult {
    std::vector<OverlapHit> hits;  ///< All overlapping entities

    /**
     * @brief Get number of overlaps
     * @return Hit count
     */
    size_t GetCount() const {
        return hits.size();
    }

    /**
     * @brief Check if any overlaps found
     * @return true if at least one overlap
     */
    bool HasOverlaps() const {
        return !hits.empty();
    }

    /**
     * @brief Get all entities
     * @return Vector of entities
     */
    std::vector<Engine::Entity> GetEntities() const {
        std::vector<Engine::Entity> entities;
        entities.reserve(hits.size());
        for (const auto &hit : hits) {
            entities.push_back(hit.entity);
        }
        return entities;
    }

    /**
     * @brief Check if specific entity overlaps
     * @param entity Entity to check
     * @return true if entity in results
     */
    bool Contains(Engine::Entity entity) const {
        for (const auto &hit : hits) {
            if (hit.entity == entity) return true;
        }
        return false;
    }

    /**
     * @brief Iterator support
     */
    auto begin() { return hits.begin(); }
    auto end() { return hits.end(); }
    auto begin() const { return hits.begin(); }
    auto end() const { return hits.end(); }
};

} // namespace Physics::Component
```

### OverlapQuery Resource

```cpp
/**************************************************************************
 * @file OverlapQuery.hpp
 * @brief Overlap query functions (spatial queries)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {
    class Core;
}

namespace Physics::Component {
    struct OverlapResult;
}

namespace Physics::Resource {

/**
 * @brief Overlap query utilities
 * 
 * Provides functions for detecting all bodies within a volume.
 * No movement required - instant spatial query.
 * 
 * **Common uses:**
 * - Explosion damage radius
 * - Proximity detection
 * - Trigger volumes
 * - Area-of-effect abilities
 * - Enemy AI awareness
 * - Physics-based UI (hover detection)
 * 
 * **Responsibilities:**
 * - Box overlap detection
 * - Sphere overlap detection
 * - Capsule overlap detection
 * - Layer filtering
 * - Efficient broad-phase queries
 */
class OverlapQuery {
public:
    /**
     * @brief Find all bodies overlapping box volume
     * @param core Engine core
     * @param center Box center position
     * @param halfExtents Box half extents
     * @param rotation Box orientation
     * @param layerMask Collision layer mask
     * @return OverlapResult with all overlapping entities
     * 
     * @example
     * @code
     * // Detect all entities in 10x10x10 box around explosion
     * glm::vec3 explosionPos(0, 5, 0);
     * auto result = Physics::Resource::OverlapQuery::OverlapBox(
     *     core,
     *     explosionPos,
     *     glm::vec3(5, 5, 5),  // 10x10x10 box
     *     glm::quat(1, 0, 0, 0)
     * );
     * 
     * for (const auto &hit : result) {
     *     // Damage entity
     *     if (hit.entity.HasComponent<Game::Health>(core)) {
     *         auto &health = hit.entity.GetComponent<Game::Health>(core);
     *         health.TakeDamage(50.0f);
     *     }
     * }
     * @endcode
     */
    static Component::OverlapResult OverlapBox(
        Engine::Core &core,
        const glm::vec3 &center,
        const glm::vec3 &halfExtents,
        const glm::quat &rotation = glm::quat(1, 0, 0, 0),
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Find all bodies overlapping sphere volume
     * @param core Engine core
     * @param center Sphere center position
     * @param radius Sphere radius
     * @param layerMask Collision layer mask
     * @return OverlapResult with all overlapping entities
     * 
     * @example
     * @code
     * // Find all enemies within 20 units of player
     * auto result = Physics::Resource::OverlapQuery::OverlapSphere(
     *     core, playerPos, 20.0f,
     *     (1u << Physics::Component::CollisionLayers::CHARACTER)
     * );
     * 
     * Logger::Info("Found {} enemies nearby", result.GetCount());
     * @endcode
     */
    static Component::OverlapResult OverlapSphere(
        Engine::Core &core,
        const glm::vec3 &center,
        float radius,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Find all bodies overlapping capsule volume
     * @param core Engine core
     * @param center Capsule center position
     * @param height Capsule height (cylinder portion)
     * @param radius Capsule radius
     * @param rotation Capsule orientation
     * @param layerMask Collision layer mask
     * @return OverlapResult with all overlapping entities
     * 
     * @example
     * @code
     * // Check if character position is occupied
     * auto result = Physics::Resource::OverlapQuery::OverlapCapsule(
     *     core, spawnPos, 1.6f, 0.3f, glm::quat(1, 0, 0, 0)
     * );
     * 
     * if (result.HasOverlaps()) {
     *     // Position occupied, can't spawn here
     * }
     * @endcode
     */
    static Component::OverlapResult OverlapCapsule(
        Engine::Core &core,
        const glm::vec3 &center,
        float height,
        float radius,
        const glm::quat &rotation = glm::quat(1, 0, 0, 0),
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Find all bodies overlapping AABB (axis-aligned box)
     * @param core Engine core
     * @param min AABB minimum corner
     * @param max AABB maximum corner
     * @param layerMask Collision layer mask
     * @return OverlapResult with all overlapping entities
     * 
     * Fast axis-aligned query (no rotation).
     * 
     * @example
     * @code
     * // Get all bodies in world region
     * auto result = Physics::Resource::OverlapQuery::OverlapAABB(
     *     core,
     *     glm::vec3(-50, 0, -50),
     *     glm::vec3(50, 100, 50)
     * );
     * @endcode
     */
    static Component::OverlapResult OverlapAABB(
        Engine::Core &core,
        const glm::vec3 &min,
        const glm::vec3 &max,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Count bodies in volume (no allocation)
     * @param core Engine core
     * @param center Sphere center
     * @param radius Sphere radius
     * @param layerMask Layer mask
     * @return Number of overlapping bodies
     * 
     * More efficient than OverlapSphere when only count is needed.
     */
    static size_t CountBodiesInSphere(
        Engine::Core &core,
        const glm::vec3 &center,
        float radius,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Check if any body overlaps volume (early out)
     * @param core Engine core
     * @param center Sphere center
     * @param radius Sphere radius
     * @param layerMask Layer mask
     * @return true if at least one overlap
     * 
     * Fastest overlap check - stops at first hit.
     */
    static bool HasAnyOverlap(
        Engine::Core &core,
        const glm::vec3 &center,
        float radius,
        uint32_t layerMask = 0xFFFFFFFF
    );

private:
    /// Convert Jolt overlap results
    static Component::OverlapResult ConvertJoltOverlap(const void *joltCollector, Engine::Core &core);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create OverlapResult (1-2 hours)

- [ ] Create `component/OverlapResult.hpp`
- [ ] Implement OverlapHit struct
- [ ] Implement OverlapResult struct
- [ ] Add GetCount(), HasOverlaps() methods
- [ ] Add GetEntities() convenience method
- [ ] Add Contains() method
- [ ] Add iterator support
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement OverlapQuery Resource (6-8 hours)

- [ ] Create `resource/OverlapQuery.hpp`
- [ ] Implement OverlapBox()
  - Create JPH::BoxShape
  - Create JPH::CollideShapeCollector
  - Query physics world
  - Convert results
- [ ] Implement OverlapSphere()
  - Create JPH::SphereShape
  - Perform overlap query
- [ ] Implement OverlapCapsule()
  - Create JPH::CapsuleShape
  - Handle orientation
- [ ] Implement OverlapAABB()
  - Fast axis-aligned query
- [ ] Implement CountBodiesInSphere()
  - Counting collector (no allocation)
- [ ] Implement HasAnyOverlap()
  - Early-out collector
- [ ] Add layer filtering

### Phase 3: Integrate with PhysicsWorldSystem (3-4 hours)

- [ ] Update `system/PhysicsWorldSystem.cpp`
- [ ] Add overlap query wrappers
- [ ] Create Jolt shapes for queries
- [ ] Use JPH::CollideShapeCollector
- [ ] Map body IDs to entities
- [ ] Apply layer filters
- [ ] Handle edge cases

### Phase 4: Unit Tests (5-6 hours)

- [ ] Create `tests/OverlapQueryTest.cpp`
- [ ] Test box overlap
- [ ] Test sphere overlap
- [ ] Test capsule overlap
- [ ] Test AABB overlap
- [ ] Test no overlaps
- [ ] Test multiple overlaps
- [ ] Test layer filtering
- [ ] Test CountBodiesInSphere
- [ ] Test HasAnyOverlap
- [ ] Test Contains() method
- [ ] Test edge cases

### Phase 5: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Explosion Damage" example
  - Sphere overlap for AOE
- [ ] Add "Proximity Detection" example
  - Find nearby enemies
- [ ] Add "Spawn Point Validation" example
  - Check if position clear
- [ ] Add "Trigger Volume" example
  - Box overlap for game logic

### Phase 6: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Overlap Queries" section
  - Explain use cases
  - Performance tips
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Box overlap finds entities
- [ ] Sphere overlap finds entities
- [ ] Capsule overlap finds entities
- [ ] AABB overlap finds entities
- [ ] No overlaps returns empty
- [ ] Multiple entities found
- [ ] Layer filtering works
- [ ] Count function accurate
- [ ] HasAnyOverlap early-exits
- [ ] Contains() works correctly
- [ ] Iterator support works

### Integration Tests

- [ ] Explosion damages all in radius
- [ ] Proximity detection finds enemies
- [ ] Spawn validation prevents overlap
- [ ] Trigger volumes activate correctly

### Performance Tests

- [ ] Box overlap: < 0.5ms (10 objects)
- [ ] Sphere overlap: < 0.3ms (10 objects)
- [ ] AABB overlap: < 0.2ms (10 objects)
- [ ] CountBodies: < 0.1ms
- [ ] HasAnyOverlap: < 0.05ms (early out)
- [ ] 100 overlaps: < 30ms

### Edge Cases

- [ ] Zero-size volume
- [ ] Very large volume (whole world)
- [ ] No bodies in world
- [ ] Overlapping thousands of bodies

## 📚 Code Examples

### Example 1: Explosion Area-of-Effect Damage

```cpp
// Damage all entities in explosion radius
void ExplodeAt(Engine::Core &core, glm::vec3 position, float radius, float damage) {
    // Find all entities in sphere
    auto result = Physics::Resource::OverlapQuery::OverlapSphere(
        core,
        position,
        radius,
        (1u << Physics::Component::CollisionLayers::DYNAMIC) |
        (1u << Physics::Component::CollisionLayers::CHARACTER)
    );
    
    Logger::Info("Explosion hit {} entities", result.GetCount());
    
    // Apply damage to each
    for (const auto &hit : result) {
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            auto &transform = hit.entity.GetComponent<Object::Component::Transform>(core);
            
            // Distance-based falloff
            float distance = glm::length(transform.position - position);
            float falloff = 1.0f - (distance / radius);
            float finalDamage = damage * falloff;
            
            health.TakeDamage(finalDamage);
            
            // Apply explosion force
            glm::vec3 direction = glm::normalize(transform.position - position);
            Physics::Resource::ForceApplicator::AddImpulse(
                core, hit.entity, direction * 500.0f * falloff
            );
        }
    }
    
    // Spawn visual effect
    SpawnExplosionEffect(core, position, radius);
}
```

### Example 2: Enemy AI Proximity Detection

```cpp
// Find nearby enemies for AI awareness
std::vector<Engine::Entity> FindNearbyEnemies(Engine::Core &core,
                                              Engine::Entity player,
                                              float detectionRadius) {
    auto &playerTransform = player.GetComponent<Object::Component::Transform>(core);
    
    // Overlap sphere around player
    auto result = Physics::Resource::OverlapQuery::OverlapSphere(
        core,
        playerTransform.position,
        detectionRadius,
        (1u << Physics::Component::CollisionLayers::CHARACTER)  // Only characters
    );
    
    std::vector<Engine::Entity> enemies;
    for (const auto &hit : result) {
        // Skip self
        if (hit.entity == player) continue;
        
        // Check if enemy
        if (hit.entity.HasComponent<Game::EnemyTag>(core)) {
            enemies.push_back(hit.entity);
        }
    }
    
    return enemies;
}
```

### Example 3: Spawn Point Validation

```cpp
// Check if spawn position is clear
bool IsSpawnPointValid(Engine::Core &core, glm::vec3 position) {
    // Character capsule dimensions
    float height = 1.6f;
    float radius = 0.3f;
    
    // Check if any body overlaps spawn position
    auto result = Physics::Resource::OverlapQuery::OverlapCapsule(
        core,
        position,
        height,
        radius,
        glm::quat(1, 0, 0, 0),
        0xFFFFFFFF  // Check all layers
    );
    
    if (result.HasOverlaps()) {
        Logger::Warn("Spawn point occupied by {} entities", result.GetCount());
        return false;
    }
    
    return true;
}

// Find nearest valid spawn point
glm::vec3 FindNearestValidSpawn(Engine::Core &core, glm::vec3 desiredPos,
                                const std::vector<glm::vec3> &candidates) {
    for (const auto &candidate : candidates) {
        if (IsSpawnPointValid(core, candidate)) {
            return candidate;
        }
    }
    
    // No valid spawn - return desired position anyway
    Logger::Error("No valid spawn points found!");
    return desiredPos;
}
```

### Example 4: Trigger Volume (Game Logic)

```cpp
// Trigger volume that activates when player enters
struct TriggerVolume {
    glm::vec3 center;
    glm::vec3 halfExtents;
    bool activated = false;
    
    void Update(Engine::Core &core, Engine::Entity player) {
        // Check if player in trigger
        auto result = Physics::Resource::OverlapQuery::OverlapBox(
            core,
            center,
            halfExtents,
            glm::quat(1, 0, 0, 0),
            (1u << Physics::Component::CollisionLayers::CHARACTER)
        );
        
        bool playerInside = result.Contains(player);
        
        if (playerInside && !activated) {
            // Player entered
            OnPlayerEnter(core, player);
            activated = true;
        } else if (!playerInside && activated) {
            // Player exited
            OnPlayerExit(core, player);
            activated = false;
        }
    }
    
    void OnPlayerEnter(Engine::Core &core, Engine::Entity player) {
        Logger::Info("Player entered trigger volume");
        // Open door, spawn enemies, etc.
    }
    
    void OnPlayerExit(Engine::Core &core, Engine::Entity player) {
        Logger::Info("Player exited trigger volume");
    }
};
```

### Example 5: Performance Optimization (Count Only)

```cpp
// Check if crowded (no need for entity details)
bool IsCrowded(Engine::Core &core, glm::vec3 position, float radius) {
    // Fast count - no entity list allocation
    size_t count = Physics::Resource::OverlapQuery::CountBodiesInSphere(
        core,
        position,
        radius,
        (1u << Physics::Component::CollisionLayers::CHARACTER)
    );
    
    return count > 5;  // Crowded if > 5 characters
}

// Check if any enemy nearby (early out)
bool HasEnemyNearby(Engine::Core &core, glm::vec3 position, float radius) {
    // Fastest check - stops at first hit
    return Physics::Resource::OverlapQuery::HasAnyOverlap(
        core,
        position,
        radius,
        (1u << Physics::Component::CollisionLayers::CHARACTER)
    );
}
```

### Example 6: Building Placement (RTS Game)

```cpp
// Check if building can be placed at position
bool CanPlaceBuilding(Engine::Core &core, glm::vec3 position,
                      glm::vec3 buildingSize) {
    // Check AABB overlap
    glm::vec3 halfExtents = buildingSize * 0.5f;
    auto result = Physics::Resource::OverlapQuery::OverlapBox(
        core,
        position,
        halfExtents,
        glm::quat(1, 0, 0, 0),
        (1u << Physics::Component::CollisionLayers::STATIC) |
        (1u << Physics::Component::CollisionLayers::DYNAMIC)
    );
    
    if (result.HasOverlaps()) {
        // Show warning to player
        for (const auto &hit : result) {
            Logger::Warn("Building blocked by: {}", entt::to_integral(hit.entity));
        }
        return false;
    }
    
    return true;
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid dimensions**
   ```cpp
   if (radius <= 0.0f) {
       Logger::Error("OverlapQuery: Invalid radius ({})", radius);
       return OverlapResult{};
   }
   
   if (halfExtents.x <= 0.0f || halfExtents.y <= 0.0f || halfExtents.z <= 0.0f) {
       Logger::Error("OverlapQuery: Invalid box extents");
       return OverlapResult{};
   }
   ```

2. **Very large query volumes (performance warning)**
   ```cpp
   float volume = (4.0f / 3.0f) * glm::pi<float>() * radius * radius * radius;
   if (volume > 1000000.0f) {
       Logger::Warn("OverlapQuery: Very large query volume ({} cubic units) may impact performance", volume);
   }
   ```

3. **Too many results (memory warning)**
   ```cpp
   if (result.GetCount() > 1000) {
       Logger::Warn("OverlapQuery: Found {} entities (large result set)", result.GetCount());
   }
   ```

4. **Physics world not initialized**
   ```cpp
   if (!physicsSystem || !physicsSystem->IsInitialized()) {
       Logger::Error("OverlapQuery: Physics world not initialized");
       return OverlapResult{};
   }
   ```

## 💡 Design Considerations

### Overlap vs Shape Cast vs Raycast

| Query Type | Movement | Speed | Use Case |
|------------|----------|-------|----------|
| **Raycast** | Line | Fastest | Line-of-sight |
| **Shape Cast** | Swept volume | Medium | Movement prediction |
| **Overlap** | None (instant) | Fast | Spatial awareness |

**When to use Overlap:**
- ✅ Area-of-effect detection
- ✅ Proximity queries
- ✅ Trigger volumes
- ✅ Spawn validation
- ✅ No movement required

### Performance Optimization

```cpp
// ✅ FAST: Count only (no allocation)
size_t count = OverlapQuery::CountBodiesInSphere(core, pos, radius);

// ✅ FAST: Early-out check
if (OverlapQuery::HasAnyOverlap(core, pos, radius)) {
    // At least one entity nearby
}

// ❌ SLOWER: Full result collection
auto result = OverlapQuery::OverlapSphere(core, pos, radius);
size_t count = result.GetCount();  // Same count, but allocated vector
```

### Layer Filtering Best Practices

```cpp
// ✅ GOOD: Filter by layer (GPU-accelerated)
uint32_t mask = (1u << CollisionLayers::DYNAMIC) |
                (1u << CollisionLayers::CHARACTER);
auto result = OverlapQuery::OverlapSphere(core, pos, radius, mask);

// ❌ BAD: Filter after query (CPU-bound)
auto result = OverlapQuery::OverlapSphere(core, pos, radius);
for (const auto &hit : result) {
    if (hit.entity.HasComponent<DynamicTag>(core)) {
        // Use hit
    }
}
```

### Trigger Volumes vs Overlap Queries

**Overlap Queries:**
- Active polling (you query every frame)
- More flexible
- Can query arbitrary positions
- Slight overhead if queried frequently

**Trigger Volumes (future #013):**
- Event-driven (callbacks on enter/exit)
- More efficient for static volumes
- Less flexible
- Zero overhead when not triggered

## 📊 Success Criteria

- [ ] OverlapResult component implemented
- [ ] OverlapQuery resource implemented
- [ ] OverlapBox() works correctly
- [ ] OverlapSphere() works correctly
- [ ] OverlapCapsule() works correctly
- [ ] OverlapAABB() works correctly
- [ ] CountBodiesInSphere() works
- [ ] HasAnyOverlap() works
- [ ] Layer filtering functional
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 0.5ms for 10 objects)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Collision Detection: https://jrouwe.github.io/JoltPhysics/
- Unity OverlapSphere: https://docs.unity3d.com/ScriptReference/Physics.OverlapSphere.html
- Unity OverlapBox: https://docs.unity3d.com/ScriptReference/Physics.OverlapBox.html
- Godot Overlap Queries: https://docs.godotengine.org/en/stable/classes/class_physicsshapequeryparameters3d.html
- Unreal Multi Sphere Trace: https://docs.unrealengine.com/5.0/en-US/BlueprintAPI/Collision/MultiSphereTraceByChannel/

## 🚀 Follow-up Issues

- #012: Advanced Collision Filtering (layers, groups, custom callbacks)
- #013: Trigger Volumes (event-driven overlap detection)
- #020: Debug Visualization (draw overlap volumes)
- #024: Spatial Queries Optimization (broad-phase caching)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.5`, `priority:high`, `queries`  
**Milestone:** v0.5 - Collision Queries
