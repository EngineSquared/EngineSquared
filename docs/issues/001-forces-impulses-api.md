# Issue #001: Implement Forces and Impulses API

**Milestone:** v0.3 - Core Completeness
**Priority:** 🔴 CRITICAL
**Estimated Effort:** 3-4 days
**Dependencies:** None
**Related Issues:** #002, #003

---

## 📋 Description

Implement a complete API for applying forces, torques, and impulses to rigid bodies. This is a critical feature for basic gameplay physics interactions (jumping, explosions, wind, etc.).

Currently, users cannot apply any forces to dynamic bodies. This issue will expose the full Jolt Physics force/impulse API through a clean, user-friendly interface.

## 🎯 Goals

- ✅ Implement force application API (continuous forces)
- ✅ Implement impulse application API (instantaneous forces)
- ✅ Support force/impulse at specific points (off-center forces)
- ✅ Support torque application (rotation forces)
- ✅ Full test coverage
- ✅ Example in physics_usage
- ✅ API documentation

## 🔧 Technical Details

### Files to Create

```
src/plugin/physics/src/resource/
├── ForceApplicator.hpp  # Force & impulse application API
└── ForceApplicator.cpp  # Implementation
```

### API Design

```cpp
namespace Physics::Resource {
    // === Forces (continuous, applied each frame) ===

    /// Apply force at center of mass
    void AddForce(Engine::Core &core, Engine::Entity entity, const glm::vec3 &force);

    /// Apply force at specific world position (creates torque)
    void AddForceAtPoint(Engine::Core &core, Engine::Entity entity,
                         const glm::vec3 &force,
                         const glm::vec3 &worldPoint);

    /// Apply torque (rotational force)
    void AddTorque(Engine::Core &core, Engine::Entity entity, const glm::vec3 &torque);


    // === Impulses (instantaneous, one-time) ===

    /// Apply impulse at center of mass
    void AddImpulse(Engine::Core &core, Engine::Entity entity, const glm::vec3 &impulse);

    /// Apply impulse at specific world position (creates angular impulse)
    void AddImpulseAtPoint(Engine::Core &core, Engine::Entity entity,
                           const glm::vec3 &impulse,
                           const glm::vec3 &worldPoint);

    /// Apply angular impulse (instantaneous rotation change)
    void AddAngularImpulse(Engine::Core &core, Engine::Entity entity,
                           const glm::vec3 &angularImpulse);
}  // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create Resource Header (1 hour)

- [ ] Create `src/plugin/physics/src/resource/ForceApplicator.hpp`
- [ ] Declare all 6 functions with full documentation (namespace `Physics::Resource`)
- [ ] Add to `Physics.hpp` public header

### Phase 2: Implement Functions (4-6 hours)

- [ ] Create `src/plugin/physics/src/resource/ForceApplicator.cpp`
- [ ] Implement `AddForce()` with error checking
  - Check entity has RigidBodyInternal
  - Get BodyInterface from PhysicsManager
  - Call JPH::BodyInterface::AddForce()
- [ ] Implement `AddForceAtPoint()` with point conversion
- [ ] Implement `AddTorque()`
- [ ] Implement `AddImpulse()`
- [ ] Implement `AddImpulseAtPoint()`
- [ ] Implement `AddAngularImpulse()`

### Phase 3: Unit Tests (3-4 hours)

- [ ] Create `src/plugin/physics/tests/ForceApplicatorTest.cpp`
- [ ] Test AddForce on dynamic body
  - Verify velocity changes
  - Verify position changes over time
- [ ] Test AddForceAtPoint creates torque
  - Apply force to corner
  - Verify body rotates
- [ ] Test AddTorque directly
- [ ] Test AddImpulse (immediate velocity change)
- [ ] Test AddImpulseAtPoint
- [ ] Test AddAngularImpulse
- [ ] Test error cases:
  - Entity without RigidBody
  - Static body (should log warning)
  - Invalid entity

### Phase 4: Integration Example (2-3 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Propelled Cube" example
  - Cube with upward force applied each frame (hovering)
- [ ] Add "Explosion" example
  - Central point applying impulses to all nearby bodies
- [ ] Add "Torque Spinner" example
  - Cube spinning from continuous torque
- [ ] Add console output showing results

### Phase 5: Documentation (1-2 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Applying Forces" section
  - Add code examples for each function
  - Explain force vs impulse
- [ ] Add Doxygen comments to all functions
- [ ] Update `examples/physics_usage/README.md`

## 🧪 Testing Checklist

### Unit Tests

- [ ] Force application increases velocity over time
- [ ] Impulse application changes velocity immediately
- [ ] Forces at points create torque (verify angular velocity)
- [ ] Torque creates rotation without translation
- [ ] Multiple forces accumulate correctly
- [ ] Forces on static bodies are ignored (with warning)

### Integration Tests

- [ ] Cube with constant upward force hovers
- [ ] Explosion impulse pushes all bodies away from center
- [ ] Torque spinner rotates continuously
- [ ] Forces work with different mass values (F = ma)

### Performance Tests

- [ ] 1000 bodies with forces applied each frame: < 5ms
- [ ] Benchmark AddForce vs AddImpulse performance

## 📚 Code Examples

### Example 1: Jump Force

```cpp
void ApplyJumpForce(Engine::Core &core, Engine::Entity player) {
    // Apply instant upward impulse (like jumping)
    glm::vec3 jumpImpulse(0.0f, 300.0f, 0.0f);
    Physics::Resource::AddImpulse(core, player, jumpImpulse);
}
```

### Example 2: Wind Force

```cpp
void ApplyWindForce(Engine::Core &core, Engine::Entity entity, float deltaTime) {
    // Continuous force (like wind)
    glm::vec3 windForce(50.0f, 0.0f, 0.0f);
    Physics::Resource::AddForce(core, entity, windForce);
}
```

### Example 3: Explosion

```cpp
void CreateExplosion(Engine::Core &core, glm::vec3 center, float radius, float strength) {
    auto view = core.GetRegistry().view<Physics::Component::RigidBody, Object::Component::Transform>();

    for (auto entity : view) {
        auto &transform = view.get<Object::Component::Transform>(entity);
        glm::vec3 toEntity = transform.position - center;
        float distance = glm::length(toEntity);

        if (distance < radius) {
            float falloff = 1.0f - (distance / radius);
            glm::vec3 impulse = glm::normalize(toEntity) * strength * falloff;
            Physics::Resource::AddImpulse(core, entity, impulse);
        }
    }
}
```

### Example 4: Torque Spinner

```cpp
void ApplySpinTorque(Engine::Core &core, Engine::Entity entity) {
    // Continuous rotation around Y axis
    glm::vec3 torque(0.0f, 10.0f, 0.0f);
    Physics::Resource::AddTorque(core, entity, torque);
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Entity without RigidBody**
   ```cpp
   if (!registry.all_of<RigidBodyInternal>(entity)) {
       Logger::Error("AddForce: Entity {} has no RigidBody component",
                     entt::to_integral(entity));
       return;
   }
   ```

2. **Static body warning**
   ```cpp
   auto &rb = registry.get<RigidBody>(entity);
   if (rb.motionType == MotionType::Static) {
       Logger::Warn("AddForce: Cannot apply force to static body (entity {})",
                    entt::to_integral(entity));
       return;
   }
   ```

3. **Invalid force values**
   ```cpp
   if (!glm::all(glm::isfinite(force))) {
       Logger::Error("AddForce: Invalid force vector (NaN or Inf)");
       return;
   }
   ```

## 📊 Success Criteria

- [ ] All 6 API functions implemented and working
- [ ] Unit tests pass (100% code coverage)
- [ ] Integration example demonstrates all functions
- [ ] Documentation complete (API + usage guide)
- [ ] Performance benchmarks acceptable
- [ ] No memory leaks (verified with Valgrind)
- [ ] Code review approved

## 🔗 References

- Jolt BodyInterface: https://jrouwe.github.io/JoltPhysics/class_body_interface.html
- Unity Rigidbody.AddForce: https://docs.unity3d.com/ScriptReference/Rigidbody.AddForce.html
- Godot RigidBody3D.apply_force: https://docs.godotengine.org/en/stable/classes/class_rigidbody3d.html#class-rigidbody3d-method-apply-force

## 🚀 Follow-up Issues

- #002: Velocity Control API
- #003: Kinematic Movement API
- #015: Advanced Force Modes (ForceMode.Acceleration, ForceMode.VelocityChange)

---

**Assignee:** TBD
**Labels:** `enhancement`, `physics`, `v0.3`, `priority:critical`
**Milestone:** v0.3 - Core Completeness
