# Issue #003: Implement Kinematic Movement API

**Status:** ✅ **DONE** (Completed 2025-11-05)  
**Milestone:** v0.3 - Core Completeness  
**Priority:** 🔴 CRITICAL  
**Estimated Effort:** 2-3 days  
**Dependencies:** None  
**Related Issues:** #001, #002

---

## 📋 Description

Implement API for moving kinematic bodies (platforms, doors, elevators, etc.). Kinematic bodies are moved programmatically but still collide with and push dynamic bodies.

This is essential for creating moving platforms, doors, elevators, and other gameplay elements that need to move predictably while interacting with physics.

## 🎯 Goals

- ✅ Implement MoveKinematic() for smooth kinematic movement
- ✅ Implement SetKinematicVelocity() helper
- ✅ Implement GetKinematicTarget() (optional)
- ✅ Full test coverage
- ✅ Example: Moving platform that carries player
- ✅ API documentation

## 🔧 Technical Details

### Files Created

```
src/plugin/physics/src/helper/
├── KinematicMover.hpp   # Kinematic movement API
└── KinematicMover.cpp   # Implementation
```

### API Design

```cpp
namespace Physics::Helper {
    /// Move kinematic body to target position/rotation over deltaTime
    /// This is the recommended way to move kinematic bodies
    /// @param targetPosition World space target position
    /// @param targetRotation World space target rotation
    /// @param deltaTime Time over which to perform the movement
    void MoveKinematic(Engine::Core &core,
                       Engine::Entity entity,
                       const glm::vec3 &targetPosition,
                       const glm::quat &targetRotation,
                       float deltaTime);

    /// Set kinematic body velocity (alternative to MoveKinematic)
    /// Useful for constant-velocity movement
    /// @param velocity World space velocity (m/s)
    void SetKinematicVelocity(Engine::Core &core,
                              Engine::Entity entity,
                              const glm::vec3 &velocity);

    /// Get kinematic body's current target (optional, for debugging)
    struct KinematicTarget {
        glm::vec3 position;
        glm::quat rotation;
        bool hasTarget;
    };
    KinematicTarget GetKinematicTarget(Engine::Core &core, Engine::Entity entity);
}  // namespace Physics::Helper
```

## 📝 Implementation Tasks

### Phase 1: Create Helper Header (1 hour)

- [x] Create `src/plugin/physics/src/helper/KinematicMover.hpp`
- [x] Declare MoveKinematic(), SetKinematicVelocity(), GetKinematicTarget() (namespace `Physics::Helper`)
- [x] Add comprehensive documentation
  - Explain kinematic vs dynamic
  - When to use each function
  - Common pitfalls
- [x] Add to `Physics.hpp`

### Phase 2: Implement MoveKinematic (2-3 hours)

- [x] Create `src/plugin/physics/src/helper/KinematicMover.cpp`
- [x] Implement MoveKinematic()
  ```cpp
  void MoveKinematic(Engine::Core &core, Engine::Entity entity,
                     const glm::vec3 &targetPosition,
                     const glm::quat &targetRotation,
                     float deltaTime) {
      auto &registry = core.GetRegistry();

      // Validate entity has RigidBody
      if (!registry.all_of<Component::RigidBody, Component::RigidBodyInternal>(entity)) {
          Log::Error("MoveKinematic: Entity has no RigidBody");
          return;
      }

      // Check it's actually kinematic
      auto &rb = registry.get<Component::RigidBody>(entity);
      if (rb.motionType != Component::MotionType::Kinematic) {
          Log::Warn("MoveKinematic: Body is not kinematic (entity {})",
                       entt::to_integral(entity));
          return;
      }

      auto &internal = registry.get<Component::RigidBodyInternal>(entity);
      auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

      // Convert to Jolt types
      JPH::RVec3 joltTarget = ToJoltVec3(targetPosition);
      JPH::Quat joltRotation = ToJoltQuat(targetRotation);

      // Move kinematic body
      physicsManager.GetBodyInterface().MoveKinematic(
          internal.bodyID,
          joltTarget,
          joltRotation,
          deltaTime
      );
  }
  ```

### Phase 3: Implement SetKinematicVelocity (1-2 hours)

- [x] Implement SetKinematicVelocity()
  ```cpp
  void SetKinematicVelocity(Engine::Core &core, Engine::Entity entity,
                            const glm::vec3 &velocity) {
      // Get current position
      auto &transform = registry.get<Object::Component::Transform>(entity);

      // Calculate target position for next frame (assuming 60 FPS)
      float deltaTime = 1.0f / 60.0f;
      glm::vec3 targetPosition = transform.position + velocity * deltaTime;

      // Move to target
      MoveKinematic(core, entity, targetPosition, transform.rotation, deltaTime);
  }
  ```

### Phase 4: Implement GetKinematicTarget (1 hour)

- [x] Implement GetKinematicTarget() for debugging
  ```cpp
  KinematicTarget GetKinematicTarget(Engine::Core &core, Engine::Entity entity) {
      // Query Jolt for current kinematic target
      // Return position, rotation, and whether target is set
  }
  ```

### Phase 5: Unit Tests (3-4 hours)

- [x] Create `src/plugin/physics/tests/helper/KinematicMoverTest.cpp`
- [x] Test MoveKinematic moves body to target
- [x] Test kinematic body pushes dynamic bodies
- [x] Test kinematic body doesn't fall (unaffected by gravity)
- [x] Test SetKinematicVelocity creates constant movement
- [x] Test kinematic collision with static bodies
- [x] Test kinematic collision with other kinematics
- [x] Test error cases:
  - MoveKinematic on dynamic body (should warn)
  - MoveKinematic on static body (should warn)
  - Invalid deltaTime (0 or negative)

### Phase 6: Integration Example (3-4 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Moving Platform" example
  - Platform moving back and forth
  - Dynamic cube placed on platform
  - Cube should move with platform
- [ ] Add "Elevator" example
  - Platform moving up and down
  - Multiple cubes on elevator
- [ ] Add "Rotating Door" example
  - Door rotating around hinge point
- [ ] Add visual output (console + potential debug rendering)

### Phase 7: Documentation (1-2 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Kinematic Bodies" section
  - Explain kinematic motion type
  - Add MoveKinematic examples
  - Add SetKinematicVelocity examples
- [ ] Create `docs/KinematicBodies.md` (detailed guide)
  - When to use kinematic vs dynamic
  - Performance considerations
  - Common patterns (platform, door, elevator)

## 🧪 Testing Checklist

### Unit Tests

- [ ] MoveKinematic smoothly interpolates position
- [ ] Kinematic body pushes dynamic bodies
- [ ] Kinematic body carries dynamic bodies (friction)
- [ ] SetKinematicVelocity creates linear motion
- [ ] Kinematic bodies don't respond to gravity
- [ ] Kinematic bodies don't respond to forces
- [ ] Invalid deltaTime is handled gracefully

### Integration Tests

- [ ] Moving platform scenario
  - Platform moves horizontally
  - Cube placed on platform
  - Cube moves with platform (not sliding off)
- [ ] Elevator scenario
  - Platform moves vertically
  - Multiple cubes on platform
  - All cubes move together
- [ ] Rotating door scenario
  - Door rotates around pivot
  - Dynamic body collides and is pushed

### Edge Cases

- [ ] Very fast kinematic movement (tunneling check)
- [ ] Very slow kinematic movement
- [ ] Kinematic body pushing heavy dynamic body
- [ ] Multiple kinematic bodies interacting
- [ ] Kinematic body squishing dynamic body

## 📚 Code Examples

### Example 1: Moving Platform (Back and Forth)

```cpp
class MovingPlatform {
    Engine::Entity platformEntity;
    glm::vec3 startPosition;
    glm::vec3 endPosition;
    float speed = 2.0f;  // m/s
    float progress = 0.0f;
    bool movingForward = true;

public:
    void Update(Engine::Core &core, float deltaTime) {
        // Update progress
        progress += (movingForward ? 1.0f : -1.0f) * speed * deltaTime /
                    glm::distance(startPosition, endPosition);

        // Reverse direction at ends
        if (progress >= 1.0f) {
            progress = 1.0f;
            movingForward = false;
        } else if (progress <= 0.0f) {
            progress = 0.0f;
            movingForward = true;
        }

        // Calculate target position
        glm::vec3 targetPosition = glm::mix(startPosition, endPosition, progress);

        // Move platform
        Physics::Helper::MoveKinematic(core, platformEntity, targetPosition,
                               glm::quat(1, 0, 0, 0), deltaTime);
    }
};
```

### Example 2: Elevator

```cpp
class Elevator {
    Engine::Entity elevatorEntity;
    float minHeight = 0.0f;
    float maxHeight = 10.0f;
    float currentHeight = 0.0f;
    float speed = 2.0f;  // m/s
    bool goingUp = true;

public:
    void Update(Engine::Core &core, float deltaTime) {
        // Update height
        currentHeight += (goingUp ? 1.0f : -1.0f) * speed * deltaTime;

        // Reverse at limits
        if (currentHeight >= maxHeight) {
            currentHeight = maxHeight;
            goingUp = false;
        } else if (currentHeight <= minHeight) {
            currentHeight = minHeight;
            goingUp = true;
        }

        // Move elevator
        glm::vec3 targetPosition(0.0f, currentHeight, 0.0f);
        Physics::Helper::MoveKinematic(core, elevatorEntity, targetPosition,
                               glm::quat(1, 0, 0, 0), deltaTime);
    }
};
```

### Example 3: Rotating Door

```cpp
class RotatingDoor {
    Engine::Entity doorEntity;
    glm::vec3 hingePoint;
    float currentAngle = 0.0f;
    float targetAngle = 0.0f;
    float rotationSpeed = glm::radians(90.0f);  // 90 deg/s

public:
    void Open() { targetAngle = glm::radians(90.0f); }
    void Close() { targetAngle = 0.0f; }

    void Update(Engine::Core &core, float deltaTime) {
        // Smoothly rotate towards target
        float angleStep = rotationSpeed * deltaTime;
        if (std::abs(targetAngle - currentAngle) < angleStep) {
            currentAngle = targetAngle;
        } else {
            currentAngle += (targetAngle > currentAngle ? angleStep : -angleStep);
        }

        // Calculate rotation around hinge
        glm::quat rotation = glm::angleAxis(currentAngle, glm::vec3(0, 1, 0));

        // Move door
        Physics::Helper::MoveKinematic(core, doorEntity, hingePoint, rotation, deltaTime);
    }
};
```

### Example 4: Constant Velocity Platform

```cpp
void UpdateConstantVelocityPlatform(Engine::Core &core, Engine::Entity platform, float deltaTime) {
    // Simple constant velocity (useful for conveyor belts)
    glm::vec3 velocity(2.0f, 0.0f, 0.0f);  // 2 m/s to the right
    Physics::Helper::SetKinematicVelocity(core, platform, velocity, deltaTime);
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Non-kinematic body**
   ```cpp
   if (rb.motionType != Component::MotionType::Kinematic) {
       Logger::Warn("MoveKinematic: Body must be kinematic (current: {})",
                    MotionTypeToString(rb.motionType));
       return;
   }
   ```

2. **Invalid deltaTime**
   ```cpp
   if (deltaTime <= 0.0f) {
       Logger::Error("MoveKinematic: deltaTime must be positive (got {})", deltaTime);
       return;
   }
   ```

3. **Invalid target values**
   ```cpp
   if (!glm::all(glm::isfinite(targetPosition))) {
       Logger::Error("MoveKinematic: Invalid target position");
       return;
   }
   ```

## 💡 Design Considerations

### Why MoveKinematic Instead of SetPosition?

**MoveKinematic** is preferred because:
- Properly handles collision with dynamic bodies
- Smooth interpolation (no tunneling)
- Jolt-optimized for kinematic movement
- Updates contact points correctly

**SetPosition** (direct) would:
- Teleport instantly (can cause tunneling)
- Miss collision events
- Not push dynamic bodies properly

### Performance Notes

- Kinematic bodies are cheaper than dynamic bodies
- Use for gameplay elements that need precise control
- Can have many kinematic bodies without performance issues
- Jolt handles kinematic-dynamic collisions efficiently

## 📊 Success Criteria

- [ ] MoveKinematic() implemented and working
- [ ] SetKinematicVelocity() helper working
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate all use cases
- [ ] Documentation complete
- [ ] Moving platform carries dynamic bodies correctly
- [ ] Performance acceptable (< 0.001ms per call)
- [ ] Code review approved

## 🔗 References

- Jolt MoveKinematic: https://jrouwe.github.io/JoltPhysics/class_body_interface.html#a8d6e6c9c3c5e7c3a0e8f8c0f0c8c8c8c
- Unity Character Controller: https://docs.unity3d.com/ScriptReference/CharacterController.html
- Godot CharacterBody3D: https://docs.godotengine.org/en/stable/classes/class_characterbody3d.html

## 🚀 Follow-up Issues

- #004: Complete RigidBody Properties
- #005: SphereCollider Component
- #017: Advanced Kinematic Patterns (splines, waypoints)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.3`, `priority:critical`  
**Milestone:** v0.3 - Core Completeness
