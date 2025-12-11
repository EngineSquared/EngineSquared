# Issue #004: Complete RigidBody Properties ✅ DONE

**Status:** ✅ COMPLETED  
**Milestone:** v0.3 - Core Completeness  
**Priority:** 🔴 CRITICAL  
**Estimated Effort:** 3-4 days  
**Dependencies:** None  
**Related Issues:** #001, #002, #003

---

## 📋 Description

Expose all essential physical properties in the `RigidBody` component. Currently, only basic properties (mass, motion type) are available. This issue adds friction, restitution, damping, gravity control, motion quality, axis locks, and continuous collision detection (CCD).

These properties are critical for realistic physics behavior and fine-grained control over rigid body dynamics.

## 🎯 Goals

- ✅ Add friction and restitution properties
- ✅ Add linear and angular damping
- ✅ Add gravity factor control
- ✅ Add motion quality settings (Discrete vs LinearCast)
- ✅ Add axis lock constraints (freeze position/rotation on specific axes)
- ✅ Add enhanced internal friction
- ✅ Add CCD (Continuous Collision Detection) settings
- ✅ Update RigidBodySystem to apply all properties
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Modify/Create

```
src/plugin/physics/src/
├── component/
│   └── RigidBody.hpp           # Add new properties
├── system/
│   └── RigidBodySystem.cpp     # Apply properties on body creation
└── tests/
    └── RigidBodyPropertiesTest.cpp  # New test file
```

### RigidBody Component Updates

```cpp
namespace Physics::Component {
    struct RigidBody {
        // === Existing properties ===
        MotionType motionType = MotionType::Dynamic;
        float mass = 1.0f;
        bool allowDynamicOrKinematic = false;
        bool isSensor = false;

        // === NEW: Material Properties ===
        float friction = 0.5f;              // Surface friction (0 = frictionless, 1 = high friction)
        float restitution = 0.0f;           // Bounciness (0 = no bounce, 1 = perfect bounce)

        // === NEW: Damping ===
        float linearDamping = 0.05f;        // Linear velocity damping (air resistance)
        float angularDamping = 0.05f;       // Angular velocity damping (rotational drag)

        // === NEW: Gravity ===
        float gravityFactor = 1.0f;         // Gravity multiplier (0 = no gravity, 2 = double gravity)

        // === NEW: Motion Quality ===
        enum class MotionQuality {
            Discrete,       // Fast, suitable for most objects
            LinearCast      // Slower but prevents tunneling for fast objects
        };
        MotionQuality motionQuality = MotionQuality::Discrete;

        // === NEW: Axis Locks ===
        bool lockPositionX = false;         // Freeze translation on X axis
        bool lockPositionY = false;         // Freeze translation on Y axis
        bool lockPositionZ = false;         // Freeze translation on Z axis
        bool lockRotationX = false;         // Freeze rotation around X axis
        bool lockRotationY = false;         // Freeze rotation around Y axis
        bool lockRotationZ = false;         // Freeze rotation around Z axis

        // === NEW: Advanced ===
        float enhancedInternalEdgeRemoval = 0.0f;  // Remove internal edges (0-1)
        bool useMotionQualityLinearCast = false;   // Enable CCD (alias for motionQuality)

        // === NEW: Sleeping ===
        bool allowSleeping = true;          // Allow body to sleep when at rest

        // === Existing Factory Methods ===
        static RigidBody CreateStatic();
        static RigidBody CreateDynamic(float mass = 1.0f);
        static RigidBody CreateKinematic();
    };
}
```

### Implementation in RigidBodySystem

Update `OnRigidBodyConstruct` to apply all properties:

```cpp
void OnRigidBodyConstruct(entt::registry &registry, entt::entity entity) {
    auto &rb = registry.get<RigidBody>(entity);
    auto &physicsManager = /* get PhysicsManager */;

    // Create BodyCreationSettings
    JPH::BodyCreationSettings bodySettings(
        /* shape, position, rotation... */
    );

    // === Apply NEW properties ===
    bodySettings.mFriction = rb.friction;
    bodySettings.mRestitution = rb.restitution;
    bodySettings.mLinearDamping = rb.linearDamping;
    bodySettings.mAngularDamping = rb.angularDamping;
    bodySettings.mGravityFactor = rb.gravityFactor;
    bodySettings.mMotionQuality = (rb.motionQuality == MotionQuality::Discrete)
        ? JPH::EMotionQuality::Discrete
        : JPH::EMotionQuality::LinearCast;
    bodySettings.mEnhancedInternalEdgeRemoval = rb.enhancedInternalEdgeRemoval > 0.0f;
    bodySettings.mAllowSleeping = rb.allowSleeping;

    // Axis locks (set allowed DOFs)
    JPH::EAllowedDOFs allowedDOFs = JPH::EAllowedDOFs::All;
    if (rb.lockPositionX || rb.lockPositionY || rb.lockPositionZ ||
        rb.lockRotationX || rb.lockRotationY || rb.lockRotationZ) {
        allowedDOFs = JPH::EAllowedDOFs::None;
        if (!rb.lockPositionX) allowedDOFs |= JPH::EAllowedDOFs::TranslationX;
        if (!rb.lockPositionY) allowedDOFs |= JPH::EAllowedDOFs::TranslationY;
        if (!rb.lockPositionZ) allowedDOFs |= JPH::EAllowedDOFs::TranslationZ;
        if (!rb.lockRotationX) allowedDOFs |= JPH::EAllowedDOFs::RotationX;
        if (!rb.lockRotationY) allowedDOFs |= JPH::EAllowedDOFs::RotationY;
        if (!rb.lockRotationZ) allowedDOFs |= JPH::EAllowedDOFs::RotationZ;
    }
    bodySettings.mAllowedDOFs = allowedDOFs;

    // Create body...
}
```

## 📝 Implementation Tasks

### Phase 1: Update RigidBody Component (2-3 hours)

- [ ] Add all new properties to `component/RigidBody.hpp`
- [ ] Add comprehensive Doxygen comments for each property
- [ ] Add MotionQuality enum
- [ ] Update factory methods if needed
- [ ] Update `Physics.hpp` if necessary

### Phase 2: Update RigidBodySystem (3-4 hours)

- [ ] Modify `OnRigidBodyConstruct` in `system/RigidBodySystem.cpp`
- [ ] Apply friction and restitution
- [ ] Apply damping values
- [ ] Apply gravity factor
- [ ] Apply motion quality
- [ ] Implement axis locks logic
- [ ] Apply enhanced internal edge removal
- [ ] Apply sleeping settings
- [ ] Handle edge cases (invalid values)

### Phase 3: Property Setters (Optional - 2-3 hours)

Create resource functions to modify properties at runtime:

- [ ] Create `src/plugin/physics/src/resource/PropertyModifier.hpp`
- [ ] Implement `SetFriction(Core&, Entity, float)`
- [ ] Implement `SetRestitution(Core&, Entity, float)`
- [ ] Implement `SetDamping(Core&, Entity, float linear, float angular)`
- [ ] Implement `SetGravityFactor(Core&, Entity, float)`
- [ ] Implement `SetAxisLock(Core&, Entity, Axis, bool locked)`

### Phase 4: Unit Tests (4-5 hours)

- [ ] Create `src/plugin/physics/tests/RigidBodyPropertiesTest.cpp`
- [ ] Test friction affects sliding behavior
- [ ] Test restitution affects bounce height
- [ ] Test damping reduces velocity over time
- [ ] Test gravity factor (0 = floating, 2 = heavy)
- [ ] Test motion quality (LinearCast prevents tunneling)
- [ ] Test axis locks (body can't move/rotate on locked axes)
- [ ] Test sleeping (inactive bodies sleep)
- [ ] Test property validation (clamp invalid values)

### Phase 5: Integration Example (2-3 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Bouncy Ball" example (high restitution)
- [ ] Add "Friction Test" example (ice vs rubber)
- [ ] Add "Damping Test" example (underwater effect)
- [ ] Add "Axis Lock" example (2D platformer constraint)
- [ ] Add "Fast Projectile" example (LinearCast motion quality)

### Phase 6: Documentation (2 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Physical Properties" section
  - Explain each property with examples
  - Add common property combinations
- [ ] Add inline code examples
- [ ] Document performance implications

## 🧪 Testing Checklist

### Unit Tests

- [ ] Friction property is applied to body
- [ ] High friction prevents sliding on slopes
- [ ] Low friction allows sliding
- [ ] Restitution = 1.0 creates perfect bounce
- [ ] Restitution = 0.0 creates no bounce
- [ ] Linear damping reduces linear velocity
- [ ] Angular damping reduces angular velocity
- [ ] Gravity factor 0.0 disables gravity
- [ ] Gravity factor 2.0 doubles gravity
- [ ] Motion quality LinearCast prevents tunneling
- [ ] Axis locks prevent movement on specified axes
- [ ] Sleeping bodies become inactive

### Integration Tests

- [ ] Bouncy ball bounces correctly
- [ ] Ice cube slides down slope
- [ ] Rubber cube sticks to slope
- [ ] Underwater objects slow down (high damping)
- [ ] Fast projectile doesn't tunnel through walls
- [ ] 2D platformer character can't rotate (locked Z rotation)
- [ ] Moon gravity (low gravity factor) works correctly

### Edge Cases

- [ ] Negative friction (clamped to 0)
- [ ] Restitution > 1.0 (clamped to 1.0)
- [ ] Very high damping (body stops quickly)
- [ ] All axes locked (body is static)
- [ ] Motion quality on static body (ignored)

## 📚 Code Examples

### Example 1: Bouncy Ball

```cpp
auto ball = Object::Resource::CreateSphere(core, 0.5f, glm::vec3(0, 10, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 1.0f;
rb.restitution = 0.9f;  // Very bouncy
rb.friction = 0.3f;
ball.AddComponent<Physics::Component::RigidBody>(core, rb);
ball.AddComponent<Physics::Component::SphereCollider>(core, {0.5f});
```

### Example 2: Ice Cube (Low Friction)

```cpp
auto iceCube = Object::Resource::CreateCube(core, 1.0f, glm::vec3(0, 5, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 1.0f;
rb.friction = 0.05f;      // Very slippery
rb.restitution = 0.1f;
iceCube.AddComponent<Physics::Component::RigidBody>(core, rb);
```

### Example 3: Underwater Object (High Damping)

```cpp
auto underwaterBox = Object::Resource::CreateCube(core, 1.0f, glm::vec3(0, 5, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 1.0f;
rb.linearDamping = 0.8f;   // Strong water resistance
rb.angularDamping = 0.8f;
underwaterBox.AddComponent<Physics::Component::RigidBody>(core, rb);
```

### Example 4: 2D Platformer Character (Axis Locks)

```cpp
auto character = Object::Resource::CreateCapsule(core, 1.0f, 0.5f, glm::vec3(0, 1, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 70.0f;
// Lock Z movement and all rotations for 2D gameplay
rb.lockPositionZ = true;
rb.lockRotationX = true;
rb.lockRotationY = true;
rb.lockRotationZ = true;
character.AddComponent<Physics::Component::RigidBody>(core, rb);
```

### Example 5: Fast Projectile (CCD)

```cpp
auto bullet = Object::Resource::CreateSphere(core, 0.1f, glm::vec3(0, 1, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 0.01f;
rb.motionQuality = Physics::Component::RigidBody::MotionQuality::LinearCast;  // Enable CCD
rb.gravityFactor = 0.5f;  // Slight gravity
bullet.AddComponent<Physics::Component::RigidBody>(core, rb);

// Launch bullet
Physics::Resource::SetLinearVelocity(core, bullet, glm::vec3(0, 0, 100));  // Very fast
```

### Example 6: Moon Gravity

```cpp
auto moonRock = Object::Resource::CreateCube(core, 1.0f, glm::vec3(0, 5, 0));

Physics::Component::RigidBody rb;
rb.motionType = Physics::Component::MotionType::Dynamic;
rb.mass = 10.0f;
rb.gravityFactor = 0.166f;  // Moon gravity (1/6 of Earth)
moonRock.AddComponent<Physics::Component::RigidBody>(core, rb);
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid friction/restitution**
   ```cpp
   if (rb.friction < 0.0f) {
       Logger::Warn("RigidBody: Friction must be >= 0, clamping to 0");
       rb.friction = 0.0f;
   }
   if (rb.restitution < 0.0f || rb.restitution > 1.0f) {
       Logger::Warn("RigidBody: Restitution must be in [0, 1], clamping");
       rb.restitution = std::clamp(rb.restitution, 0.0f, 1.0f);
   }
   ```

2. **Invalid damping**
   ```cpp
   if (rb.linearDamping < 0.0f || rb.linearDamping > 1.0f) {
       Logger::Warn("RigidBody: Linear damping should be in [0, 1]");
   }
   ```

3. **Axis locks on static body**
   ```cpp
   if (rb.motionType == MotionType::Static && (rb.lockPositionX || /* ... */)) {
       Logger::Info("RigidBody: Axis locks ignored for static bodies");
   }
   ```

## 💡 Design Considerations

### Property Defaults

Default values should match Jolt Physics defaults and provide sensible behavior:
- **Friction 0.5**: Moderate grip (neither ice nor glue)
- **Restitution 0.0**: No bounce (most objects don't bounce)
- **Damping 0.05**: Slight air resistance
- **Gravity factor 1.0**: Normal Earth gravity
- **Motion quality Discrete**: Fast, suitable for most objects
- **Sleeping true**: Performance optimization

### Motion Quality Performance

- **Discrete**: Fast, O(n) complexity, suitable for 99% of objects
- **LinearCast**: Slower, O(n²) complexity, only for very fast objects (bullets, etc.)

**Use LinearCast only when:**
- Object moves faster than its size per frame
- Tunneling is a problem
- Precision is critical

### Axis Locks vs Constraints

Axis locks are **lightweight** compared to constraints:
- ✅ Zero overhead (handled by solver)
- ✅ Simple to use
- ✅ Perfect for 2D games, platformers

Use **constraints** (Issue #010+) for:
- Springs, hinges, motors
- Complex joint behavior
- Breakable connections

## 📊 Success Criteria

- [x] All properties added to RigidBody component
- [x] Properties correctly applied in RigidBodySystem
- [x] Unit tests pass (100% coverage)
- [x] Integration examples demonstrate all properties
- [x] Documentation complete with examples
- [x] Performance acceptable (no regression)
- [x] Code review approved

## 🔗 References

- Jolt BodyCreationSettings: https://jrouwe.github.io/JoltPhysics/class_body_creation_settings.html
- Unity Rigidbody properties: https://docs.unity3d.com/ScriptReference/Rigidbody.html
- Godot RigidBody3D: https://docs.godotengine.org/en/stable/classes/class_rigidbody3d.html
- Physics materials: https://en.wikipedia.org/wiki/Friction#Coefficient_of_friction

## 🚀 Follow-up Issues

- #005: SphereCollider & CapsuleCollider
- #006: Runtime property modification API (PropertyModifier resource)
- #010: Constraints & Joints (advanced axis control)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.3`, `priority:critical`  
**Milestone:** v0.3 - Core Completeness
