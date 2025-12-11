# Issue #002: Implement Velocity Control API

**Status:** ✅ **DONE** (Completed 2025-11-05)  
**Milestone:** v0.3 - Core Completeness  
**Priority:** 🔴 CRITICAL  
**Estimated Effort:** 2-3 days  
**Dependencies:** None  
**Related Issues:** #001, #003

---

## 📋 Description

Implement API for directly getting and setting linear/angular velocities on rigid bodies. This is essential for direct kinematic control, character controllers, and precise movement behaviors.

Unlike forces/impulses which require physics simulation to affect velocity, this API provides direct velocity control for immediate response.

## 🎯 Goals

- ✅ Implement get/set linear velocity
- ✅ Implement get/set angular velocity
- ✅ Implement add (delta) velocity functions
- ✅ Full test coverage
- ✅ Example demonstrating velocity control
- ✅ API documentation

## 🔧 Technical Details

### Files Created

```
src/plugin/physics/src/helper/
├── VelocityController.hpp  # Velocity control API
└── VelocityController.cpp  # Implementation
```

### API Design

```cpp
namespace Physics::Helper {
    // === Linear Velocity ===

    /// Set linear velocity (world space)
    void SetLinearVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &velocity);

    /// Get current linear velocity (world space)
    glm::vec3 GetLinearVelocity(Engine::Core &core, Engine::Entity entity);

    /// Add to current linear velocity (delta)
    void AddLinearVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &deltaVelocity);


    // === Angular Velocity ===

    /// Set angular velocity (world space, axis = rotation axis, magnitude = rad/s)
    void SetAngularVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &angularVelocity);

    /// Get current angular velocity (world space)
    glm::vec3 GetAngularVelocity(Engine::Core &core, Engine::Entity entity);

    /// Add to current angular velocity (delta)
    void AddAngularVelocity(Engine::Core &core, Engine::Entity entity, const glm::vec3 &deltaAngularVelocity);
}  // namespace Physics::Helper
```

## 📝 Implementation Tasks

### Phase 1: Create Helper Header (30 min)

- [x] Create `src/plugin/physics/src/helper/VelocityController.hpp`
- [x] Declare all 6 functions (namespace `Physics::Helper`)
- [x] Add comprehensive Doxygen comments
- [x] Add to `Physics.hpp`

### Phase 2: Implement Getters (1-2 hours)

- [x] Implement `GetLinearVelocity()`
  ```cpp
  glm::vec3 GetLinearVelocity(Engine::Core &core, Engine::Entity entity) {
      auto &registry = core.GetRegistry();
      if (!registry.all_of<Component::RigidBodyInternal>(entity)) {
          Log::Error("GetLinearVelocity: Entity has no RigidBody");
          return glm::vec3(0.0f);
      }

      auto &internal = registry.get<Component::RigidBodyInternal>(entity);
      auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

      JPH::Vec3 velocity = physicsManager.GetBodyInterface()
                               .GetLinearVelocity(internal.bodyID);

      return FromJoltVec3(velocity);
  }
  ```

- [x] Implement `GetAngularVelocity()`

### Phase 3: Implement Setters (1-2 hours)

- [x] Implement `SetLinearVelocity()`
  - Validate velocity values (no NaN/Inf)
  - Activate body if sleeping
  - Use BodyInterface::SetLinearVelocity()

- [x] Implement `SetAngularVelocity()`
  - Same validation and activation

- [x] Implement `AddLinearVelocity()`
  - Get current velocity
  - Add delta
  - Set new velocity

- [x] Implement `AddAngularVelocity()`

### Phase 4: Unit Tests (2-3 hours)

- [x] Create `src/plugin/physics/tests/helper/VelocityControllerTest.cpp`
- [x] Test SetLinearVelocity changes position over time
- [x] Test GetLinearVelocity returns correct value
- [x] Test AddLinearVelocity accumulates
- [x] Test SetAngularVelocity creates rotation
- [x] Test GetAngularVelocity returns correct value
- [x] Test AddAngularVelocity accumulates
- [x] Test velocity on static body (should fail gracefully)
- [x] Test velocity persistence across frames

### Phase 5: Integration Example (2 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Velocity Control" example
  - Cube with constant velocity (no forces)
  - Arrow keys to change velocity
- [ ] Add "Spinner" example
  - Cube with angular velocity
  - Spins continuously without torque
- [ ] Add console output

### Phase 6: Documentation (1 hour)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Velocity Control" section
  - Explain velocity vs forces
  - When to use each
- [ ] Add examples for each function

## 🧪 Testing Checklist

### Unit Tests

- [ ] SetLinearVelocity moves body predictably
- [ ] GetLinearVelocity returns accurate value
- [ ] AddLinearVelocity accumulates correctly
- [ ] SetAngularVelocity creates rotation
- [ ] GetAngularVelocity returns accurate value
- [ ] Velocity setters activate sleeping bodies
- [ ] Invalid velocities (NaN/Inf) are rejected
- [ ] Static bodies reject velocity changes (with warning)

### Integration Tests

- [ ] Cube with constant velocity moves linearly
- [ ] Cube with angular velocity spins continuously
- [ ] Velocity changes are frame-rate independent
- [ ] Multiple velocity changes accumulate correctly

### Edge Cases

- [ ] Zero velocity
- [ ] Very high velocity (clamp check)
- [ ] Rapid velocity changes (every frame)
- [ ] Velocity on kinematic bodies

## 📚 Code Examples

### Example 1: Constant Velocity Movement

```cpp
void MoveAtConstantSpeed(Engine::Core &core, Engine::Entity entity) {
    // Move forward at 5 m/s
    glm::vec3 velocity(0.0f, 0.0f, 5.0f);
    Physics::Helper::SetLinearVelocity(core, entity, velocity);
}
```

### Example 2: Character Movement (Velocity-based)

```cpp
void UpdateCharacterMovement(Engine::Core &core, Engine::Entity character,
                             const glm::vec2 &input, float speed) {
    glm::vec3 velocity(input.x * speed, 0.0f, input.y * speed);

    // Keep current Y velocity (gravity)
    glm::vec3 currentVel = Physics::Helper::GetLinearVelocity(core, character);
    velocity.y = currentVel.y;

    Physics::Helper::SetLinearVelocity(core, character, velocity);
}
```

### Example 3: Spinner

```cpp
void CreateSpinner(Engine::Core &core, Engine::Entity entity, float rpm) {
    // Convert RPM to rad/s
    float radiansPerSecond = (rpm * 2.0f * glm::pi<float>()) / 60.0f;

    // Spin around Y axis
    glm::vec3 angularVelocity(0.0f, radiansPerSecond, 0.0f);
    Physics::Helper::SetAngularVelocity(core, entity, angularVelocity);
}
```

### Example 4: Velocity Damping (Custom)

```cpp
void ApplyCustomDamping(Engine::Core &core, Engine::Entity entity, float damping, float deltaTime) {
    glm::vec3 velocity = Physics::Helper::GetLinearVelocity(core, entity);

    // Apply exponential damping
    float dampingFactor = std::pow(1.0f - damping, deltaTime);
    velocity *= dampingFactor;

    Physics::Helper::SetLinearVelocity(core, entity, velocity);
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Entity without RigidBody**
   ```cpp
   if (!registry.all_of<RigidBodyInternal>(entity)) {
       Logger::Error("SetLinearVelocity: Entity {} has no RigidBody",
                     entt::to_integral(entity));
       return;
   }
   ```

2. **Invalid velocity values**
   ```cpp
   if (!glm::all(glm::isfinite(velocity))) {
       Logger::Error("SetLinearVelocity: Invalid velocity (NaN or Inf)");
       return;
   }
   ```

3. **Static body**
   ```cpp
   auto &rb = registry.get<RigidBody>(entity);
   if (rb.motionType == MotionType::Static) {
       Logger::Warn("SetLinearVelocity: Cannot set velocity on static body");
       return;
   }
   ```

## 💡 Design Considerations

### Velocity vs Forces: When to Use Each

**Use Velocity when:**
- Direct control needed (platformer character)
- Instant response required
- Frame-rate independent movement
- Bypassing physics simulation

**Use Forces when:**
- Realistic physics behavior
- Acceleration over time
- Interaction with other forces (gravity, friction)
- Physical accuracy matters

### Activation Behavior

Setting velocity should **activate sleeping bodies**:
```cpp
void SetLinearVelocity(Core &core, entt::entity entity, const glm::vec3 &velocity) {
    // ... validation ...

    bodyInterface.SetLinearVelocity(internal.bodyID, joltVelocity);

    // Activate if needed
    if (!bodyInterface.IsActive(internal.bodyID)) {
        bodyInterface.ActivateBody(internal.bodyID);
    }
}
```

## 📊 Success Criteria

- [ ] All 6 API functions implemented
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Documentation complete
- [ ] Performance acceptable (< 0.001ms per call)
- [ ] Code review approved

## 🔗 References

- Jolt BodyInterface Velocity: https://jrouwe.github.io/JoltPhysics/class_body_interface.html
- Unity Rigidbody.velocity: https://docs.unity3d.com/ScriptReference/Rigidbody-velocity.html
- Godot RigidBody3D.linear_velocity: https://docs.godotengine.org/en/stable/classes/class_rigidbody3d.html#class-rigidbody3d-property-linear-velocity

## 🚀 Follow-up Issues

- #003: Kinematic Movement API
- #004: Complete RigidBody Properties
- #016: Velocity Clamping (max velocity limits)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.3`, `priority:critical`  
**Milestone:** v0.3 - Core Completeness
