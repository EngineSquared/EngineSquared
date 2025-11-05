# Issue #010: Implement Shape Cast System

**Milestone:** v0.5 - Collision Queries  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 3-4 days  
**Dependencies:** #009 (Raycast)  
**Related Issues:** #011 (Overlap), #012 (Filtering)

---

## 📋 Description

Implement shape casting (swept shape queries) for collision detection. Shape casts extend raycasting by sweeping a 3D shape along a path, essential for character movement prediction, area-of-effect checking, and thick raycasts.

Shape casts solve problems where raycasts are too thin (miss small gaps) or need volume-based detection.

## 🎯 Goals

- ✅ Implement ShapeCast API (box, sphere, capsule sweep)
- ✅ Support all collider shapes
- ✅ Provide detailed hit information
- ✅ Layer-based filtering
- ✅ Rotation during sweep
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── ShapeCastQuery.hpp      # New file
│   └── ShapeCastQuery.cpp      # New file
├── component/
│   └── RaycastHit.hpp          # Extend for shape casts
└── tests/
    └── ShapeCastTest.cpp       # New file
```

### Extend RaycastHit for Shape Casts

```cpp
// Update component/RaycastHit.hpp
namespace Physics::Component {

/**
 * @brief Shape cast hit information
 * 
 * Extends RaycastHit with shape-specific data.
 * Contains all information about a shape cast collision.
 */
struct ShapeCastHit : public RaycastHit {
    glm::vec3 penetrationAxis = glm::vec3(0.0f);  ///< Penetration direction
    float penetrationDepth = 0.0f;                 ///< Penetration depth (if overlapping)
    
    /**
     * @brief Check if shapes are overlapping at start
     * @return true if start position overlaps
     */
    bool IsInitialOverlap() const {
        return distance == 0.0f && penetrationDepth > 0.0f;
    }
};

} // namespace Physics::Component
```

### ShapeCastQuery Resource

```cpp
/**************************************************************************
 * @file ShapeCastQuery.hpp
 * @brief Shape cast query functions (swept shape collision detection)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Engine {
    class Core;
}

namespace Physics::Component {
    struct ShapeCastHit;
    struct BoxCollider;
    struct SphereCollider;
    struct CapsuleCollider;
}

namespace Physics::Resource {

/**
 * @brief Shape cast query utilities
 * 
 * Provides functions for sweeping shapes through space to detect collisions.
 * More accurate than raycasts for volumetric queries.
 * 
 * **Common uses:**
 * - Character movement prediction (swept capsule)
 * - Thick raycasts (avoid thin-ray misses)
 * - Area-of-effect detection
 * - Vehicle collision prediction
 * - Projectile with radius
 * 
 * **Responsibilities:**
 * - Box sweep
 * - Sphere sweep
 * - Capsule sweep
 * - Layer filtering
 * - Rotation during sweep (optional)
 */
class ShapeCastQuery {
public:
    /**
     * @brief Sweep box shape along direction
     * @param core Engine core
     * @param halfExtents Box half extents
     * @param origin Start position (box center)
     * @param rotation Box orientation
     * @param direction Sweep direction (will be normalized)
     * @param maxDistance Maximum sweep distance
     * @param layerMask Collision layer mask
     * @return ShapeCastHit result
     * 
     * @example
     * @code
     * // Check if character can move forward (swept box)
     * glm::vec3 boxSize(0.5f, 1.0f, 0.5f);
     * auto hit = Physics::Resource::ShapeCastQuery::CastBox(
     *     core, boxSize, playerPos, playerRot,
     *     glm::vec3(0, 0, 1), 2.0f  // Try move 2 units forward
     * );
     * if (!hit) {
     *     // Path clear, can move
     *     playerPos += glm::vec3(0, 0, 1) * 2.0f;
     * }
     * @endcode
     */
    static Component::ShapeCastHit CastBox(
        Engine::Core &core,
        const glm::vec3 &halfExtents,
        const glm::vec3 &origin,
        const glm::quat &rotation,
        const glm::vec3 &direction,
        float maxDistance = 1000.0f,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Sweep sphere shape along direction
     * @param core Engine core
     * @param radius Sphere radius
     * @param origin Start position (sphere center)
     * @param direction Sweep direction
     * @param maxDistance Maximum sweep distance
     * @param layerMask Collision layer mask
     * @return ShapeCastHit result
     * 
     * @example
     * @code
     * // Thick raycast for projectile with radius
     * auto hit = Physics::Resource::ShapeCastQuery::CastSphere(
     *     core, 0.1f, projectilePos, projectileDir, 50.0f
     * );
     * @endcode
     */
    static Component::ShapeCastHit CastSphere(
        Engine::Core &core,
        float radius,
        const glm::vec3 &origin,
        const glm::vec3 &direction,
        float maxDistance = 1000.0f,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Sweep capsule shape along direction
     * @param core Engine core
     * @param height Capsule height (cylinder height, excluding hemispheres)
     * @param radius Capsule radius
     * @param origin Start position (capsule center)
     * @param rotation Capsule orientation
     * @param direction Sweep direction
     * @param maxDistance Maximum sweep distance
     * @param layerMask Collision layer mask
     * @return ShapeCastHit result
     * 
     * @example
     * @code
     * // Character controller collision prediction
     * auto hit = Physics::Resource::ShapeCastQuery::CastCapsule(
     *     core, 1.6f, 0.3f,  // Height 1.6m, radius 0.3m
     *     characterPos, characterRot,
     *     moveDir, moveDistance
     * );
     * @endcode
     */
    static Component::ShapeCastHit CastCapsule(
        Engine::Core &core,
        float height,
        float radius,
        const glm::vec3 &origin,
        const glm::quat &rotation,
        const glm::vec3 &direction,
        float maxDistance = 1000.0f,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Sweep shape with rotation (advanced)
     * @param core Engine core
     * @param shape Shape to sweep (Box, Sphere, or Capsule)
     * @param startPos Start position
     * @param startRot Start rotation
     * @param endPos End position
     * @param endRot End rotation (interpolated during sweep)
     * @param layerMask Collision layer mask
     * @return ShapeCastHit result
     * 
     * Sweeps shape from start to end with rotation interpolation.
     * Useful for complex movements (e.g., rolling objects).
     * 
     * @note More expensive than linear sweep
     */
    static Component::ShapeCastHit CastShapeRotating(
        Engine::Core &core,
        const Component::BoxCollider &shape,
        const glm::vec3 &startPos,
        const glm::quat &startRot,
        const glm::vec3 &endPos,
        const glm::quat &endRot,
        uint32_t layerMask = 0xFFFFFFFF
    );

    /**
     * @brief Cast shape and ignore specific entity
     * @param core Engine core
     * @param shape Shape to cast
     * @param origin Start position
     * @param rotation Orientation
     * @param direction Sweep direction
     * @param maxDistance Maximum distance
     * @param ignoreEntity Entity to ignore
     * @param layerMask Layer mask
     * @return ShapeCastHit result
     */
    template<typename ShapeType>
    static Component::ShapeCastHit CastShapeIgnore(
        Engine::Core &core,
        const ShapeType &shape,
        const glm::vec3 &origin,
        const glm::quat &rotation,
        const glm::vec3 &direction,
        float maxDistance,
        Engine::Entity ignoreEntity,
        uint32_t layerMask = 0xFFFFFFFF
    );

private:
    /// Convert Jolt shape cast result
    static Component::ShapeCastHit ConvertJoltShapeCast(const void *joltHit, Engine::Core &core);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Extend RaycastHit for ShapeCasts (1-2 hours)

- [ ] Update `component/RaycastHit.hpp`
- [ ] Add ShapeCastHit struct (extends RaycastHit)
- [ ] Add penetrationAxis field
- [ ] Add penetrationDepth field
- [ ] Add IsInitialOverlap() method
- [ ] Update Doxygen comments

### Phase 2: Implement ShapeCastQuery (8-10 hours)

- [ ] Create `resource/ShapeCastQuery.hpp`
- [ ] Implement CastBox()
  - Create JPH::BoxShape
  - Create JPH::ShapeCast
  - Convert result to ShapeCastHit
- [ ] Implement CastSphere()
  - Create JPH::SphereShape
  - Perform shape cast
- [ ] Implement CastCapsule()
  - Create JPH::CapsuleShape
  - Handle orientation
- [ ] Implement CastShapeRotating()
  - Interpolate rotation along path
  - Create JPH::ShapeCast with rotation
- [ ] Implement CastShapeIgnore()
  - Filter results by entity
- [ ] Add layer filtering
- [ ] Handle edge cases

### Phase 3: Integrate with PhysicsWorldSystem (3-4 hours)

- [ ] Update `system/PhysicsWorldSystem.cpp`
- [ ] Add shape cast wrapper functions
- [ ] Create Jolt shapes from parameters
- [ ] Perform JPH::CastShape queries
- [ ] Map results back to entities
- [ ] Apply layer filters

### Phase 4: Unit Tests (6-7 hours)

- [ ] Create `tests/ShapeCastTest.cpp`
- [ ] Test box sweep hit
- [ ] Test sphere sweep hit
- [ ] Test capsule sweep hit
- [ ] Test sweep miss
- [ ] Test initial overlap detection
- [ ] Test distance accuracy
- [ ] Test normal calculation
- [ ] Test penetration depth
- [ ] Test layer filtering
- [ ] Test rotating sweep
- [ ] Test ignore entity

### Phase 5: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Character Movement Prediction" example
  - Swept capsule before moving
- [ ] Add "Thick Raycast" example
  - Sphere sweep vs normal ray
- [ ] Add "Vehicle Collision Prediction" example
  - Box sweep for vehicle
- [ ] Add "Area Damage" example
  - Sphere sweep for explosion radius

### Phase 6: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Shape Casting" section
  - When to use shape cast vs raycast
  - Performance considerations
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Box sweep hits object
- [ ] Sphere sweep hits object
- [ ] Capsule sweep hits object
- [ ] Shape sweep misses
- [ ] Hit point is accurate
- [ ] Hit normal is accurate
- [ ] Distance is correct
- [ ] Initial overlap detected
- [ ] Penetration depth calculated
- [ ] Layer filtering works
- [ ] Rotating sweep works
- [ ] Ignore entity works

### Integration Tests

- [ ] Character movement prediction prevents clipping
- [ ] Thick raycast catches small objects
- [ ] Vehicle prediction works
- [ ] Sphere sweep for explosions

### Performance Tests

- [ ] Box cast: < 0.2ms
- [ ] Sphere cast: < 0.1ms
- [ ] Capsule cast: < 0.2ms
- [ ] 100 shape casts: < 15ms
- [ ] Rotating sweep: < 0.5ms

### Edge Cases

- [ ] Zero distance sweep
- [ ] Sweep from inside object
- [ ] Very long sweep (1000+ units)
- [ ] Sweep parallel to surface

## 📚 Code Examples

### Example 1: Character Movement Prediction (Swept Capsule)

```cpp
// Predict character movement before committing
void TryMoveCharacter(Engine::Core &core, Engine::Entity character,
                      glm::vec3 moveDirection, float moveDistance) {
    auto &transform = character.GetComponent<Object::Component::Transform>(core);
    
    // Character capsule dimensions
    float height = 1.6f;
    float radius = 0.3f;
    
    // Sweep capsule along movement path
    auto hit = Physics::Resource::ShapeCastQuery::CastCapsule(
        core,
        height, radius,
        transform.position,
        transform.rotation,
        moveDirection,
        moveDistance,
        0xFFFFFFFF
    );
    
    if (!hit) {
        // Path clear - move full distance
        transform.position += moveDirection * moveDistance;
    } else if (hit.distance > 0.01f) {
        // Partial movement possible - slide along surface
        float safeDistance = hit.distance - 0.01f;  // Small safety margin
        transform.position += moveDirection * safeDistance;
        
        // Slide along hit normal
        glm::vec3 slideDir = glm::normalize(
            moveDirection - glm::dot(moveDirection, hit.normal) * hit.normal
        );
        // Recursively try sliding movement...
    } else {
        // Stuck - can't move
        Logger::Warn("Character stuck at position: {}", transform.position);
    }
}
```

### Example 2: Thick Raycast (Projectile with Radius)

```cpp
// Projectile that has physical size (not infinitely thin ray)
void FireThickProjectile(Engine::Core &core, glm::vec3 origin,
                         glm::vec3 direction, float radius) {
    // Use sphere sweep instead of ray
    auto hit = Physics::Resource::ShapeCastQuery::CastSphere(
        core,
        radius,        // Projectile radius
        origin,
        direction,
        100.0f,        // Max range
        0xFFFFFFFF
    );
    
    if (hit) {
        Logger::Info("Hit at distance: {}", hit.distance);
        
        // Apply damage
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            health.TakeDamage(50.0f);
        }
        
        // Spawn explosion at impact
        SpawnExplosion(core, hit.point, hit.normal);
    }
}
```

### Example 3: Vehicle Collision Prediction

```cpp
// Check if vehicle can move forward
bool CanVehicleMove(Engine::Core &core, Engine::Entity vehicle,
                    glm::vec3 moveDirection, float moveDistance) {
    auto &transform = vehicle.GetComponent<Object::Component::Transform>(core);
    
    // Vehicle bounding box
    glm::vec3 halfExtents(1.5f, 0.8f, 2.5f);  // Width, height, length
    
    // Sweep box along movement
    auto hit = Physics::Resource::ShapeCastQuery::CastBox(
        core,
        halfExtents,
        transform.position,
        transform.rotation,
        moveDirection,
        moveDistance,
        (1u << Physics::Component::CollisionLayers::STATIC) |
        (1u << Physics::Component::CollisionLayers::DYNAMIC)
    );
    
    if (hit && hit.distance < 0.5f) {
        // Too close to obstacle
        return false;
    }
    
    return true;
}
```

### Example 4: Area-of-Effect Damage (Expanding Sphere)

```cpp
// Explosion that expands over time
void ExpandingExplosion(Engine::Core &core, glm::vec3 center,
                        float maxRadius, float duration) {
    float currentRadius = 0.0f;
    float radiusPerSecond = maxRadius / duration;
    
    // Each frame, sweep sphere from last radius to current radius
    float dt = GetDeltaTime();
    float newRadius = currentRadius + radiusPerSecond * dt;
    
    // Sweep spherical shell
    auto hit = Physics::Resource::ShapeCastQuery::CastSphere(
        core,
        newRadius,
        center,
        glm::vec3(0.0f),  // No direction (just expand)
        newRadius - currentRadius,  // Shell thickness
        (1u << Physics::Component::CollisionLayers::CHARACTER) |
        (1u << Physics::Component::CollisionLayers::DYNAMIC)
    );
    
    if (hit) {
        // Damage entity in expansion wave
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            float damage = 100.0f * (1.0f - currentRadius / maxRadius);
            health.TakeDamage(damage);
        }
    }
    
    currentRadius = newRadius;
}
```

### Example 5: Rotating Sweep (Rolling Boulder)

```cpp
// Boulder that rotates while rolling down hill
void RollingBoulder(Engine::Core &core, glm::vec3 startPos, glm::vec3 endPos,
                    float radius) {
    // Calculate rotation based on distance
    float distance = glm::length(endPos - startPos);
    float rotationAngle = distance / radius;  // Radians
    
    glm::quat startRot = glm::quat(1, 0, 0, 0);
    glm::quat endRot = glm::angleAxis(rotationAngle, glm::vec3(1, 0, 0));
    
    // Create temporary box for shape cast (boulder approximation)
    Physics::Component::BoxCollider boulderBox(glm::vec3(radius));
    
    // Sweep with rotation
    auto hit = Physics::Resource::ShapeCastQuery::CastShapeRotating(
        core,
        boulderBox,
        startPos,
        startRot,
        endPos,
        endRot,
        0xFFFFFFFF
    );
    
    if (hit) {
        // Boulder hits something - apply damage and stop
        Logger::Info("Boulder hit at: {}", hit.point);
        
        if (hit.entity.HasComponent<Game::Health>(core)) {
            auto &health = hit.entity.GetComponent<Game::Health>(core);
            health.TakeDamage(150.0f);  // Heavy damage!
        }
    }
}
```

### Example 6: Initial Overlap Detection

```cpp
// Check if shape overlaps anything at current position
bool IsPositionClear(Engine::Core &core, glm::vec3 position, float radius) {
    // Sweep with zero distance to check overlap
    auto hit = Physics::Resource::ShapeCastQuery::CastSphere(
        core,
        radius,
        position,
        glm::vec3(0, 1, 0),  // Arbitrary direction
        0.01f,               // Near-zero distance
        0xFFFFFFFF
    );
    
    if (hit.IsInitialOverlap()) {
        // Position is occupied
        Logger::Warn("Position occupied: penetration depth = {}", hit.penetrationDepth);
        return false;
    }
    
    return true;  // Position clear
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid shape parameters**
   ```cpp
   if (radius <= 0.0f) {
       Logger::Error("ShapeCastQuery: Invalid radius ({})", radius);
       return ShapeCastHit{};
   }
   
   if (halfExtents.x <= 0.0f || halfExtents.y <= 0.0f || halfExtents.z <= 0.0f) {
       Logger::Error("ShapeCastQuery: Invalid box extents");
       return ShapeCastHit{};
   }
   ```

2. **Invalid direction (for directional sweeps)**
   ```cpp
   if (glm::length(direction) < 0.001f) {
       Logger::Error("ShapeCastQuery: Invalid direction (zero length)");
       return ShapeCastHit{};
   }
   ```

3. **Negative distance**
   ```cpp
   if (maxDistance < 0.0f) {
       Logger::Error("ShapeCastQuery: Negative distance");
       return ShapeCastHit{};
   }
   ```

4. **Initial overlap warnings**
   ```cpp
   if (hit.IsInitialOverlap()) {
       Logger::Warn("ShapeCastQuery: Shape starts inside collision (penetration: {})",
                    hit.penetrationDepth);
   }
   ```

## 💡 Design Considerations

### Shape Cast vs Raycast

| Feature | Raycast | Shape Cast |
|---------|---------|------------|
| **Speed** | Fastest | Slower |
| **Accuracy** | Thin line | Volumetric |
| **Use Case** | Line-of-sight, bullets | Character movement, thick projectiles |
| **Misses Small Gaps** | Yes | No |
| **Cost** | ~0.1ms | ~0.2-0.5ms |

**When to use Shape Cast:**
- ✅ Character collision prediction
- ✅ Vehicle movement
- ✅ Projectiles with radius
- ✅ Area queries with movement
- ✅ Avoiding thin-ray artifacts

**When to use Raycast:**
- ✅ Line-of-sight checks
- ✅ Hitscan weapons
- ✅ Simple distance queries
- ✅ Performance-critical scenarios

### Rotation During Sweep

**Linear sweep (no rotation):**
- Faster
- Simpler
- Good for most cases

**Rotating sweep:**
- More accurate
- More expensive (2-3x)
- Needed for realistic rolling/tumbling

### Initial Overlap Handling

When shape starts inside collision:
```cpp
if (hit.IsInitialOverlap()) {
    // Option 1: Reject movement
    return false;
    
    // Option 2: Push out along penetration axis
    position += hit.penetrationAxis * hit.penetrationDepth;
    
    // Option 3: Allow movement (sliding)
    // Use penetration info to calculate slide direction
}
```

## 📊 Success Criteria

- [ ] ShapeCastHit struct implemented
- [ ] ShapeCastQuery resource implemented
- [ ] CastBox() works correctly
- [ ] CastSphere() works correctly
- [ ] CastCapsule() works correctly
- [ ] CastShapeRotating() works
- [ ] Layer filtering functional
- [ ] Initial overlap detection works
- [ ] Penetration depth accurate
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 0.5ms)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt ShapeCast: https://jrouwe.github.io/JoltPhysics/
- Unity SphereCast: https://docs.unity3d.com/ScriptReference/Physics.SphereCast.html
- Unity BoxCast: https://docs.unity3d.com/ScriptReference/Physics.BoxCast.html
- Godot ShapeCast: https://docs.godotengine.org/en/stable/classes/class_shapecast3d.html
- Swept Collision Detection: https://www.gamedeveloper.com/programming/swept-aabb-collision-detection-and-response

## 🚀 Follow-up Issues

- #011: Overlap Queries (spatial queries without movement)
- #012: Advanced Collision Filtering
- #020: Debug Visualization (draw shape casts)
- #023: Continuous Collision Detection (CCD integration)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.5`, `priority:high`, `queries`  
**Milestone:** v0.5 - Collision Queries
