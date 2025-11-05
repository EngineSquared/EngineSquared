# Issue #005: Implement SphereCollider and CapsuleCollider

**Milestone:** v0.3 - Core Completeness  
**Priority:** 🔴 CRITICAL  
**Estimated Effort:** 3-4 days  
**Dependencies:** None  
**Related Issues:** #004

---

## 📋 Description

Implement `SphereCollider` and `CapsuleCollider` components to complement the existing `BoxCollider`. These are the most commonly used primitive colliders in game engines and are essential for characters, projectiles, and basic geometry.

Currently, only `BoxCollider` and `DefaultCollider` are available. This limits the types of physics objects that can be created efficiently.

## 🎯 Goals

- ✅ Implement SphereCollider component
- ✅ Implement CapsuleCollider component
- ✅ Update RigidBodySystem to handle new colliders
- ✅ Update collider dispatcher in shape creation
- ✅ Full test coverage
- ✅ Integration with Object plugin (CreateSphere, CreateCapsule helpers)
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── SphereCollider.hpp       # New file
│   ├── SphereCollider.cpp       # New file
│   ├── CapsuleCollider.hpp      # New file
│   └── CapsuleCollider.cpp      # New file
├── system/
│   └── RigidBodySystem.cpp      # Update shape dispatcher
├── resource/
│   └── ShapeFactory.hpp         # (Optional) Shape creation helpers
└── tests/
    ├── SphereColliderTest.cpp   # New file
    └── CapsuleColliderTest.cpp  # New file
```

### SphereCollider Component

```cpp
/**************************************************************************
 * @file SphereCollider.hpp
 * @brief Sphere collision shape component
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Sphere collision shape
 * 
 * A spherical collision shape defined by a single radius.
 * Spheres are the most efficient collision shape and should be preferred
 * when the object's shape can be approximated as a sphere.
 * 
 * Common uses:
 * - Balls, projectiles
 * - Spherical triggers
 * - Particle systems
 * - Simplified character collision
 * 
 * @note Spheres have uniform collision response in all directions
 * @note Most efficient primitive for broad phase and narrow phase collision detection
 */
struct SphereCollider {
    float radius = 0.5f;  ///< Sphere radius in world units

    /**
     * @brief Construct sphere collider with specified radius
     * @param r Radius (must be > 0)
     */
    explicit SphereCollider(float r = 0.5f) : radius(r) {}

    /**
     * @brief Factory method for sphere collider
     * @param radius Sphere radius (default: 0.5)
     * @return SphereCollider instance
     * 
     * @example
     * @code
     * auto sphere = Physics::Component::SphereCollider::Create(1.0f);
     * entity.AddComponent<Physics::Component::SphereCollider>(core, sphere);
     * @endcode
     */
    static SphereCollider Create(float radius = 0.5f) {
        return SphereCollider(radius);
    }

    /**
     * @brief Validate sphere collider properties
     * @return true if valid, false otherwise
     */
    bool IsValid() const {
        return radius > 0.0f;
    }

    /**
     * @brief Get sphere volume
     * @return Volume in cubic units
     */
    float GetVolume() const {
        return (4.0f / 3.0f) * glm::pi<float>() * radius * radius * radius;
    }

    /**
     * @brief Get sphere surface area
     * @return Surface area in square units
     */
    float GetSurfaceArea() const {
        return 4.0f * glm::pi<float>() * radius * radius;
    }
};

} // namespace Physics::Component
```

### CapsuleCollider Component

```cpp
/**************************************************************************
 * @file CapsuleCollider.hpp
 * @brief Capsule collision shape component
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Capsule collision shape (cylinder with hemispherical caps)
 * 
 * A capsule is defined by a height (cylinder length) and radius.
 * The total height includes the two hemispherical caps.
 * Capsules are ideal for character controllers and elongated objects.
 * 
 * Capsule orientation: Y-axis aligned (vertical by default)
 * 
 * Common uses:
 * - Character controllers (humanoids, NPCs)
 * - Elongated projectiles (rockets, arrows)
 * - Triggers (corridors, zones)
 * 
 * @note More efficient than mesh colliders for character collision
 * @note Provides smooth collision response (no snagging on edges)
 */
struct CapsuleCollider {
    float height = 1.0f;  ///< Cylinder height (excluding caps)
    float radius = 0.5f;  ///< Capsule radius

    /**
     * @brief Construct capsule collider
     * @param h Cylinder height (excluding caps)
     * @param r Capsule radius
     */
    CapsuleCollider(float h = 1.0f, float r = 0.5f) 
        : height(h), radius(r) {}

    /**
     * @brief Factory method for capsule collider
     * @param height Cylinder height (default: 1.0)
     * @param radius Capsule radius (default: 0.5)
     * @return CapsuleCollider instance
     * 
     * @example
     * @code
     * // Create character capsule: 1.8m tall, 0.3m radius
     * auto capsule = Physics::Component::CapsuleCollider::Create(1.8f, 0.3f);
     * entity.AddComponent<Physics::Component::CapsuleCollider>(core, capsule);
     * @endcode
     */
    static CapsuleCollider Create(float height = 1.0f, float radius = 0.5f) {
        return CapsuleCollider(height, radius);
    }

    /**
     * @brief Validate capsule collider properties
     * @return true if valid, false otherwise
     */
    bool IsValid() const {
        return height > 0.0f && radius > 0.0f;
    }

    /**
     * @brief Get total capsule height (cylinder + caps)
     * @return Total height in world units
     */
    float GetTotalHeight() const {
        return height + 2.0f * radius;  // Cylinder + two hemispheres
    }

    /**
     * @brief Get capsule volume
     * @return Volume in cubic units
     */
    float GetVolume() const {
        // Cylinder volume + sphere volume
        float cylinderVolume = glm::pi<float>() * radius * radius * height;
        float sphereVolume = (4.0f / 3.0f) * glm::pi<float>() * radius * radius * radius;
        return cylinderVolume + sphereVolume;
    }

    /**
     * @brief Get capsule surface area
     * @return Surface area in square units
     */
    float GetSurfaceArea() const {
        // Cylinder surface + sphere surface
        float cylinderArea = 2.0f * glm::pi<float>() * radius * height;
        float sphereArea = 4.0f * glm::pi<float>() * radius * radius;
        return cylinderArea + sphereArea;
    }
};

} // namespace Physics::Component
```

### Update Shape Dispatcher

In `system/RigidBodySystem.cpp`, update the collider dispatcher:

```cpp
JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
    // Priority order: Explicit collider > Default collider
    
    // 1. Try BoxCollider
    if (registry.all_of<Component::BoxCollider>(entity)) {
        auto &box = registry.get<Component::BoxCollider>(entity);
        return new JPH::BoxShapeSettings(Resource::ToJoltVec3(box.halfExtents));
    }
    
    // 2. Try SphereCollider (NEW)
    if (registry.all_of<Component::SphereCollider>(entity)) {
        auto &sphere = registry.get<Component::SphereCollider>(entity);
        return new JPH::SphereShapeSettings(sphere.radius);
    }
    
    // 3. Try CapsuleCollider (NEW)
    if (registry.all_of<Component::CapsuleCollider>(entity)) {
        auto &capsule = registry.get<Component::CapsuleCollider>(entity);
        return new JPH::CapsuleShapeSettings(capsule.height * 0.5f, capsule.radius);
    }
    
    // 4. Fallback to DefaultCollider
    return CreateDefaultShape(registry, entity);
}
```

## 📝 Implementation Tasks

### Phase 1: Create SphereCollider (2-3 hours)

- [ ] Create `src/plugin/physics/src/component/SphereCollider.hpp`
- [ ] Implement SphereCollider struct with radius property
- [ ] Add factory method `Create(radius)`
- [ ] Add validation methods
- [ ] Add helper methods (GetVolume, GetSurfaceArea)
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create CapsuleCollider (2-3 hours)

- [ ] Create `src/plugin/physics/src/component/CapsuleCollider.hpp`
- [ ] Implement CapsuleCollider struct with height and radius
- [ ] Add factory method `Create(height, radius)`
- [ ] Add validation methods
- [ ] Add helper methods (GetTotalHeight, GetVolume, GetSurfaceArea)
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 3: Update RigidBodySystem (2-3 hours)

- [ ] Update `GetColliderShape` dispatcher in `system/RigidBodySystem.cpp`
- [ ] Add SphereCollider case
  - Convert radius to Jolt
  - Create JPH::SphereShapeSettings
- [ ] Add CapsuleCollider case
  - Convert height/radius to Jolt
  - Create JPH::CapsuleShapeSettings
  - Handle orientation (Y-axis default)
- [ ] Test collider priority order

### Phase 4: Unit Tests (4-5 hours)

- [ ] Create `src/plugin/physics/tests/SphereColliderTest.cpp`
  - Test sphere creation
  - Test sphere collision detection
  - Test sphere rolling behavior
  - Test invalid radius (negative, zero)
- [ ] Create `src/plugin/physics/tests/CapsuleColliderTest.cpp`
  - Test capsule creation
  - Test capsule collision detection
  - Test capsule as character controller
  - Test invalid dimensions

### Phase 5: Integration with Object Plugin (2-3 hours)

Update Object plugin helpers to work with new colliders:

- [ ] Test `Object::Resource::CreateSphere` with `SphereCollider`
- [ ] Add `Object::Resource::CreateCapsule` helper (if not exists)
- [ ] Verify mesh-collider alignment

### Phase 6: Integration Examples (3-4 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Rolling Sphere" example
  - Sphere with SphereCollider
  - Apply force to roll
  - Demonstrate smooth rolling
- [ ] Add "Character Controller" example
  - Capsule with CapsuleCollider
  - Move with velocity control
  - No snagging on edges
- [ ] Add "Projectile" example
  - Small sphere with high velocity
  - Demonstrate LinearCast motion quality

### Phase 7: Documentation (2 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Collider Types" section
  - Document SphereCollider
  - Document CapsuleCollider
  - Add comparison table (Box vs Sphere vs Capsule)
- [ ] Add usage examples for each collider type

## 🧪 Testing Checklist

### SphereCollider Tests

- [ ] Sphere collider creates valid Jolt shape
- [ ] Sphere radius property is applied
- [ ] Sphere collides with other shapes
- [ ] Sphere rolls smoothly down slope
- [ ] Invalid radius is rejected (≤ 0)
- [ ] Sphere volume calculation is correct
- [ ] Sphere surface area calculation is correct

### CapsuleCollider Tests

- [ ] Capsule collider creates valid Jolt shape
- [ ] Capsule height and radius are applied
- [ ] Capsule collides with other shapes
- [ ] Capsule orientation is correct (Y-axis)
- [ ] Invalid dimensions are rejected (≤ 0)
- [ ] Capsule doesn't snag on edges (character movement)
- [ ] Total height calculation is correct (height + 2*radius)

### Integration Tests

- [ ] Sphere vs Box collision
- [ ] Sphere vs Sphere collision
- [ ] Capsule vs Box collision
- [ ] Capsule vs Sphere collision
- [ ] Capsule as character controller (stairs, slopes)
- [ ] Multiple collider types in same scene

### Performance Tests

- [ ] 1000 spheres: < 5ms per frame
- [ ] 100 capsules: < 2ms per frame
- [ ] Collider creation overhead: < 0.1ms each

## 📚 Code Examples

### Example 1: Rolling Ball

```cpp
auto ball = Object::Resource::CreateSphere(core, 0.5f, glm::vec3(0, 10, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 1.0f;
rb.friction = 0.5f;
rb.restitution = 0.6f;  // Bouncy
ball.AddComponent<Physics::Component::RigidBody>(core, rb);

// Use SphereCollider for perfect rolling
Physics::Component::SphereCollider sphereCol(0.5f);
ball.AddComponent<Physics::Component::SphereCollider>(core, sphereCol);

// Apply initial force
Physics::Resource::AddImpulse(core, ball, glm::vec3(5, 0, 0));
```

### Example 2: Character Controller

```cpp
auto character = Object::Resource::CreateCapsule(core, 1.8f, 0.3f, glm::vec3(0, 1, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 70.0f;  // Average human mass
// Lock rotations for character controller
rb.lockRotationX = true;
rb.lockRotationY = true;
rb.lockRotationZ = true;
rb.friction = 0.0f;  // We'll control movement directly
character.AddComponent<Physics::Component::RigidBody>(core, rb);

// CapsuleCollider: 1.8m tall character with 0.3m radius
Physics::Component::CapsuleCollider capsuleCol(1.8f, 0.3f);
character.AddComponent<Physics::Component::CapsuleCollider>(core, capsuleCol);

// Control movement with velocity
void UpdateCharacter(Engine::Core &core, Engine::Entity character, glm::vec2 input) {
    glm::vec3 velocity(input.x * 5.0f, 0.0f, input.y * 5.0f);
    glm::vec3 currentVel = Physics::Resource::GetLinearVelocity(core, character);
    velocity.y = currentVel.y;  // Preserve gravity
    Physics::Resource::SetLinearVelocity(core, character, velocity);
}
```

### Example 3: Projectile (Fast Sphere)

```cpp
auto bullet = Object::Resource::CreateSphere(core, 0.1f, glm::vec3(0, 1, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 0.01f;
rb.motionQuality = Physics::Component::RigidBody::MotionQuality::LinearCast;  // CCD
rb.gravityFactor = 0.2f;
bullet.AddComponent<Physics::Component::RigidBody>(core, rb);

Physics::Component::SphereCollider sphereCol(0.1f);
bullet.AddComponent<Physics::Component::SphereCollider>(core, sphereCol);

// Launch at high speed
Physics::Resource::SetLinearVelocity(core, bullet, glm::vec3(0, 5, 100));
```

### Example 4: Trigger Volume (Sphere)

```cpp
auto triggerZone = core.CreateEntity();

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Static;
rb.isSensor = true;  // Trigger, no collision response
triggerZone.AddComponent<Physics::Component::RigidBody>(core, rb);

// Large sphere trigger
Physics::Component::SphereCollider sphereCol(5.0f);
triggerZone.AddComponent<Physics::Component::SphereCollider>(core, sphereCol);

// Handle trigger events
void OnTriggerEnter(const Physics::TriggerEnterEvent &event) {
    if (event.self == triggerZone) {
        Logger::Info("Entity entered trigger zone!");
    }
}
```

### Example 5: Collider Comparison

```cpp
// Box: Best for rectangular objects, walls, floors
auto box = Object::Resource::CreateCube(core, 1.0f, glm::vec3(0, 5, 0));
box.AddComponent<Physics::Component::RigidBody>(core, /* ... */);
// BoxCollider is default for CreateCube

// Sphere: Best for balls, particles, radial symmetry
auto sphere = Object::Resource::CreateSphere(core, 0.5f, glm::vec3(5, 5, 0));
sphere.AddComponent<Physics::Component::RigidBody>(core, /* ... */);
Physics::Component::SphereCollider sphereCol(0.5f);
sphere.AddComponent<Physics::Component::SphereCollider>(core, sphereCol);

// Capsule: Best for characters, elongated objects
auto capsule = Object::Resource::CreateCapsule(core, 1.8f, 0.3f, glm::vec3(10, 1, 0));
capsule.AddComponent<Physics::Component::RigidBody>(core, /* ... */);
Physics::Component::CapsuleCollider capsuleCol(1.8f, 0.3f);
capsule.AddComponent<Physics::Component::CapsuleCollider>(core, capsuleCol);
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid sphere radius**
   ```cpp
   if (sphere.radius <= 0.0f) {
       Logger::Error("SphereCollider: Radius must be > 0 (got {})", sphere.radius);
       return nullptr;  // Fallback to DefaultCollider
   }
   ```

2. **Invalid capsule dimensions**
   ```cpp
   if (capsule.height <= 0.0f || capsule.radius <= 0.0f) {
       Logger::Error("CapsuleCollider: Height and radius must be > 0");
       return nullptr;
   }
   ```

3. **Multiple colliders on same entity**
   ```cpp
   // Warn if multiple explicit colliders (ambiguous)
   int colliderCount = 0;
   if (registry.all_of<BoxCollider>(entity)) colliderCount++;
   if (registry.all_of<SphereCollider>(entity)) colliderCount++;
   if (registry.all_of<CapsuleCollider>(entity)) colliderCount++;
   if (colliderCount > 1) {
       Logger::Warn("Entity has multiple colliders, using priority order: Box > Sphere > Capsule");
   }
   ```

## 💡 Design Considerations

### Collider Priority Order

When multiple colliders exist on an entity:
1. **BoxCollider** (most explicit, manual setup)
2. **SphereCollider** (simple primitive)
3. **CapsuleCollider** (complex primitive)
4. **DefaultCollider** (fallback)

This order prioritizes user intent (explicit > implicit).

### Capsule Orientation

Jolt uses **Y-axis aligned** capsules by default:
- Y-axis = "up" direction (character standing)
- For horizontal capsules, use rotation in Transform

### Performance

**Collision detection cost (fastest to slowest):**
1. Sphere-Sphere (trivial distance check)
2. Sphere-Box (SAT with sphere optimization)
3. Capsule-Sphere (line-sphere distance)
4. Capsule-Box (complex SAT)
5. Capsule-Capsule (line-line distance)

**Choose collider based on:**
- Sphere: When shape is roughly spherical
- Capsule: When shape is elongated (characters, cylinders)
- Box: When shape has flat surfaces

## 📊 Success Criteria

- [ ] SphereCollider implemented and working
- [ ] CapsuleCollider implemented and working
- [ ] RigidBodySystem handles both colliders
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Character controller example works smoothly
- [ ] Documentation complete
- [ ] Performance acceptable (no regression)
- [ ] Code review approved

## 🔗 References

- Jolt SphereShape: https://jrouwe.github.io/JoltPhysics/class_sphere_shape.html
- Jolt CapsuleShape: https://jrouwe.github.io/JoltPhysics/class_capsule_shape.html
- Unity SphereCollider: https://docs.unity3d.com/ScriptReference/SphereCollider.html
- Unity CapsuleCollider: https://docs.unity3d.com/ScriptReference/CapsuleCollider.html
- Godot SphereShape3D: https://docs.godotengine.org/en/stable/classes/class_sphereshape3d.html
- Godot CapsuleShape3D: https://docs.godotengine.org/en/stable/classes/class_capsuleshape3d.html

## 🚀 Follow-up Issues

- #006: MeshCollider (complex geometry)
- #007: CompoundCollider (multiple shapes)
- #008: HeightFieldCollider (terrain)
- #015: Character Controller System (built on CapsuleCollider)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.3`, `priority:critical`  
**Milestone:** v0.3 - Core Completeness
