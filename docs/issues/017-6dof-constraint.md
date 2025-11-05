# Issue #017: Implement 6DOF Constraint (Advanced Configuration)

**Milestone:** v0.6 - Collision Events & Constraints  
**Priority:** 🟢 MEDIUM  
**Estimated Effort:** 3-4 days  
**Dependencies:** #015 (Basic Constraints), #016 (Hinge/Slider)  
**Related Issues:** #023 (Performance Profiling), #020 (Ragdoll System)

---

## 📋 Description

Implement 6-Degrees-of-Freedom (6DOF) constraint, the most flexible physics constraint. Allows independent configuration of all 3 translation axes and 3 rotation axes with limits, springs, and motors.

This constraint is the "universal joint" - can simulate any other constraint type and enables complex mechanical systems.

## 🎯 Goals

- ✅ Implement SixDofConstraint component
- ✅ Per-axis configuration (translation X, Y, Z)
- ✅ Per-axis configuration (rotation X, Y, Z)
- ✅ Axis limits (min/max for each axis)
- ✅ Axis motors (force/torque per axis)
- ✅ Axis springs (stiffness/damping per axis)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   └── SixDofConstraint.hpp        # New file
├── resource/
│   ├── ConstraintFactory.hpp       # Modify (add 6DOF)
│   └── ConstraintFactory.cpp       # Modify
├── system/
│   ├── ConstraintSystem.hpp        # Modify
│   └── ConstraintSystem.cpp        # Modify
└── tests/
    └── SixDofConstraintTest.cpp    # New file
```

### SixDofConstraint Component

```cpp
/**************************************************************************
 * @file SixDofConstraint.hpp
 * @brief Six-degrees-of-freedom constraint (universal joint)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Motion type for axis
 */
enum class AxisMotion {
    Locked,   ///< Axis is completely locked (no motion)
    Limited,  ///< Axis has min/max limits
    Free      ///< Axis is free (no limits)
};

/**
 * @brief Motor settings for single axis
 */
struct AxisMotor {
    bool enabled = false;           ///< Enable motor
    float targetVelocity = 0.0f;    ///< Target velocity (m/s or rad/s)
    float targetPosition = 0.0f;    ///< Target position (meters or radians)
    float maxForce = 0.0f;          ///< Maximum force/torque (N or Nm)
    bool positionMode = false;      ///< Position servo mode (vs velocity)
    
    /**
     * @brief Create velocity motor
     * @param velocity Target velocity
     * @param force Max force
     * @return AxisMotor
     */
    static AxisMotor Velocity(float velocity, float force) {
        return {true, velocity, 0.0f, force, false};
    }
    
    /**
     * @brief Create position servo
     * @param position Target position
     * @param force Max force
     * @return AxisMotor
     */
    static AxisMotor Position(float position, float force) {
        return {true, 0.0f, position, force, true};
    }
    
    /**
     * @brief Disable motor
     * @return AxisMotor
     */
    static AxisMotor None() {
        return {false, 0.0f, 0.0f, 0.0f, false};
    }
};

/**
 * @brief Spring settings for single axis
 */
struct AxisSpring {
    bool enabled = false;      ///< Enable spring
    float stiffness = 0.0f;    ///< Spring stiffness [0, 1]
    float damping = 0.0f;      ///< Damping coefficient [0, 1]
    float targetPosition = 0.0f; ///< Spring rest position
    
    /**
     * @brief Create spring
     * @param stiff Stiffness [0, 1]
     * @param damp Damping [0, 1]
     * @param target Rest position
     * @return AxisSpring
     */
    static AxisSpring Create(float stiff, float damp, float target = 0.0f) {
        return {true, stiff, damp, target};
    }
    
    /**
     * @brief Disable spring
     * @return AxisSpring
     */
    static AxisSpring None() {
        return {false, 0.0f, 0.0f, 0.0f};
    }
};

/**
 * @brief Configuration for single axis (translation or rotation)
 */
struct AxisConfig {
    AxisMotion motion = AxisMotion::Locked;  ///< Motion type
    float minLimit = 0.0f;                   ///< Minimum limit
    float maxLimit = 0.0f;                   ///< Maximum limit
    AxisMotor motor = AxisMotor::None();     ///< Motor settings
    AxisSpring spring = AxisSpring::None();  ///< Spring settings
    
    /// Current state (read-only)
    float currentPosition = 0.0f;  ///< Current position/angle
    float currentVelocity = 0.0f;  ///< Current velocity
    
    /**
     * @brief Create locked axis (no motion)
     * @return AxisConfig
     */
    static AxisConfig Locked() {
        return {AxisMotion::Locked, 0.0f, 0.0f};
    }
    
    /**
     * @brief Create free axis (unlimited motion)
     * @return AxisConfig
     */
    static AxisConfig Free() {
        return {AxisMotion::Free, 0.0f, 0.0f};
    }
    
    /**
     * @brief Create limited axis
     * @param minLim Minimum limit
     * @param maxLim Maximum limit
     * @return AxisConfig
     */
    static AxisConfig Limited(float minLim, float maxLim) {
        return {AxisMotion::Limited, minLim, maxLim};
    }
    
    /**
     * @brief Create motorized axis
     * @param motorSettings Motor settings
     * @return AxisConfig
     */
    static AxisConfig WithMotor(const AxisMotor &motorSettings) {
        AxisConfig c = Free();
        c.motor = motorSettings;
        return c;
    }
    
    /**
     * @brief Create spring axis
     * @param springSettings Spring settings
     * @return AxisConfig
     */
    static AxisConfig WithSpring(const AxisSpring &springSettings) {
        AxisConfig c = Free();
        c.spring = springSettings;
        return c;
    }
};

/**
 * @brief Six-degrees-of-freedom constraint
 * 
 * Most flexible constraint. Allows independent control of all 6 axes:
 * - Translation: X, Y, Z
 * - Rotation: X (roll), Y (pitch), Z (yaw)
 * 
 * Each axis can be:
 * - Locked (no motion)
 * - Limited (min/max range)
 * - Free (unlimited)
 * - Motorized (velocity or position control)
 * - Spring-damped (soft constraint)
 * 
 * **Common uses:**
 * - Ragdoll joints (shoulder, hip, neck)
 * - Complex mechanical systems
 * - Vehicle suspension
 * - Character controller
 * - Simulate any other constraint type
 * 
 * **Physics:**
 * - 0-6 constraints (depends on locked axes)
 * - 0-6 degrees of freedom
 * - Most expensive constraint (but most flexible)
 * 
 * @example
 * @code
 * // Ragdoll shoulder (ball joint with limits)
 * Physics::Component::SixDofConstraint shoulder;
 * shoulder.bodyA = torso;
 * shoulder.bodyB = upperArm;
 * 
 * // Lock all translations (fixed point)
 * shoulder.translationX = AxisConfig::Locked();
 * shoulder.translationY = AxisConfig::Locked();
 * shoulder.translationZ = AxisConfig::Locked();
 * 
 * // Free rotations with limits (realistic shoulder)
 * shoulder.rotationX = AxisConfig::Limited(-1.5f, 1.5f);  // Forward/back
 * shoulder.rotationY = AxisConfig::Limited(-1.0f, 2.0f);  // Up/down
 * shoulder.rotationZ = AxisConfig::Limited(-0.5f, 0.5f);  // Twist
 * 
 * torso.AddComponent<Physics::Component::SixDofConstraint>(core, shoulder);
 * @endcode
 */
struct SixDofConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /// Reference frames (local space rotations)
    glm::quat rotationA = glm::quat(1, 0, 0, 0);  ///< Frame A rotation
    glm::quat rotationB = glm::quat(1, 0, 0, 0);  ///< Frame B rotation
    
    /// Translation axes (X, Y, Z)
    AxisConfig translationX = AxisConfig::Locked();
    AxisConfig translationY = AxisConfig::Locked();
    AxisConfig translationZ = AxisConfig::Locked();
    
    /// Rotation axes (Roll, Pitch, Yaw)
    AxisConfig rotationX = AxisConfig::Free();
    AxisConfig rotationY = AxisConfig::Free();
    AxisConfig rotationZ = AxisConfig::Free();
    
    /**
     * @brief Create 6DOF constraint (all locked)
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param settings Constraint settings
     * @return SixDofConstraint
     */
    static SixDofConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        SixDofConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create ball joint (point constraint with rotation freedom)
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @return SixDofConstraint
     */
    static SixDofConstraint CreateBallJoint(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f)
    ) {
        SixDofConstraint c = Create(a, b, pointOnA, pointOnB);
        // Lock all translations
        c.translationX = AxisConfig::Locked();
        c.translationY = AxisConfig::Locked();
        c.translationZ = AxisConfig::Locked();
        // Free all rotations
        c.rotationX = AxisConfig::Free();
        c.rotationY = AxisConfig::Free();
        c.rotationZ = AxisConfig::Free();
        return c;
    }
    
    /**
     * @brief Create limited ball joint (ragdoll joint)
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param angleLimit Maximum angle from neutral (radians)
     * @return SixDofConstraint
     */
    static SixDofConstraint CreateRagdollJoint(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        float angleLimit = 1.57f  // 90 degrees default
    ) {
        SixDofConstraint c = CreateBallJoint(a, b, pointOnA, pointOnB);
        // Add symmetric limits to rotations
        c.rotationX = AxisConfig::Limited(-angleLimit, angleLimit);
        c.rotationY = AxisConfig::Limited(-angleLimit, angleLimit);
        c.rotationZ = AxisConfig::Limited(-angleLimit * 0.5f, angleLimit * 0.5f);  // Less twist
        return c;
    }
    
    /**
     * @brief Create vehicle suspension (spring on Y, locked X/Z)
     * @param a Chassis body
     * @param b Wheel body
     * @param pointOnA Attachment point on chassis
     * @param pointOnB Attachment point on wheel
     * @param springStiffness Spring stiffness [0, 1]
     * @param springDamping Damping [0, 1]
     * @param travelDistance Suspension travel (meters)
     * @return SixDofConstraint
     */
    static SixDofConstraint CreateSuspension(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        float springStiffness,
        float springDamping,
        float travelDistance
    ) {
        SixDofConstraint c = Create(a, b, pointOnA, pointOnB);
        // Lock X/Z translation
        c.translationX = AxisConfig::Locked();
        c.translationZ = AxisConfig::Locked();
        // Y translation with spring and limits
        c.translationY = AxisConfig::Limited(-travelDistance, 0.0f);
        c.translationY.spring = AxisSpring::Create(springStiffness, springDamping, 0.0f);
        // Lock all rotations (wheel has separate hinge)
        c.rotationX = AxisConfig::Locked();
        c.rotationY = AxisConfig::Locked();
        c.rotationZ = AxisConfig::Locked();
        return c;
    }
    
    /**
     * @brief Set motor velocity for axis
     * @param axis Axis index (0-5: transX, transY, transZ, rotX, rotY, rotZ)
     * @param velocity Target velocity
     */
    void SetAxisMotorVelocity(int axis, float velocity) {
        AxisConfig *config = GetAxisConfig(axis);
        if (config) {
            config->motor.enabled = true;
            config->motor.positionMode = false;
            config->motor.targetVelocity = velocity;
        }
    }
    
    /**
     * @brief Set motor target position for axis
     * @param axis Axis index (0-5)
     * @param position Target position
     */
    void SetAxisMotorPosition(int axis, float position) {
        AxisConfig *config = GetAxisConfig(axis);
        if (config) {
            config->motor.enabled = true;
            config->motor.positionMode = true;
            config->motor.targetPosition = position;
        }
    }
    
    /**
     * @brief Disable motor for axis
     * @param axis Axis index (0-5)
     */
    void DisableAxisMotor(int axis) {
        AxisConfig *config = GetAxisConfig(axis);
        if (config) {
            config->motor.enabled = false;
        }
    }

private:
    AxisConfig *GetAxisConfig(int axis) {
        switch (axis) {
            case 0: return &translationX;
            case 1: return &translationY;
            case 2: return &translationZ;
            case 3: return &rotationX;
            case 4: return &rotationY;
            case 5: return &rotationZ;
            default: return nullptr;
        }
    }
};

} // namespace Physics::Component
```

### ConstraintFactory Update

```cpp
// Add to ConstraintFactory.hpp

namespace Physics::Resource {

class ConstraintFactory {
public:
    // ... existing methods ...
    
    /**
     * @brief Create Jolt 6DOF constraint
     * @param core Engine core
     * @param constraint SixDofConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreateSixDof(Engine::Core &core,
                                  const Component::SixDofConstraint &constraint);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create 6DOF Component (4-5 hours)

- [ ] Create `component/SixDofConstraint.hpp`
- [ ] Implement AxisMotion enum
- [ ] Implement AxisMotor struct
- [ ] Implement AxisSpring struct
- [ ] Implement AxisConfig struct
- [ ] Implement SixDofConstraint struct
- [ ] Add factory methods (Create/CreateBallJoint/CreateRagdollJoint/CreateSuspension)
- [ ] Add motor control methods
- [ ] Add Doxygen comments with examples
- [ ] Add to `Physics.hpp`

### Phase 2: Implement 6DOF Factory (10-12 hours)

- [ ] Update `resource/ConstraintFactory.hpp`
- [ ] Implement CreateSixDof()
  - Create JPH::SixDOFConstraintSettings
  - Configure each axis (Locked/Limited/Free)
  - Set min/max limits per axis
  - Configure motors per axis
  - Configure springs per axis
  - Add to Jolt world
- [ ] Handle axis state updates
- [ ] Update current position/velocity

### Phase 3: Update ConstraintSystem (3-4 hours)

- [ ] Update `system/ConstraintSystem.hpp`
- [ ] Add ProcessNewSixDofConstraints()
- [ ] Update motor settings per axis
- [ ] Update spring settings per axis
- [ ] Update current state per axis

### Phase 4: Unit Tests (10-12 hours)

- [ ] Create `tests/SixDofConstraintTest.cpp`
- [ ] Test locked axes (no motion)
- [ ] Test limited axes (respect limits)
- [ ] Test free axes (unlimited)
- [ ] Test ball joint (rotations only)
- [ ] Test ragdoll joint (limited rotations)
- [ ] Test suspension (spring + limits)
- [ ] Test motors per axis
- [ ] Test springs per axis

### Phase 5: Integration Examples (10-12 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Ball Joint" example (free rotation)
- [ ] Add "Ragdoll Joint" example (limited rotation)
- [ ] Add "Vehicle Suspension" example (spring + limits)
- [ ] Add "Custom Constraint" example (mixed axes)
- [ ] Add "Motorized 6DOF" example (per-axis motors)

### Phase 6: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "6DOF Constraints" section
  - Explain axis configuration
  - Explain motors and springs per axis
  - Comparison with other constraint types
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Locked axes prevent motion
- [ ] Limited axes respect min/max
- [ ] Free axes allow unlimited motion
- [ ] Ball joint rotates freely
- [ ] Ragdoll joint respects angle limits
- [ ] Suspension spring works
- [ ] Per-axis motors work
- [ ] Per-axis springs work
- [ ] Motor+spring combination works

### Integration Tests

- [ ] Ball joint allows free rotation
- [ ] Ragdoll joint mimics real limb
- [ ] Vehicle suspension absorbs bumps
- [ ] Custom constraint behaves as expected

### Performance Tests

- [ ] 50 6DOF constraints: < 5ms overhead
- [ ] Per-axis motor: < 1ms overhead

### Edge Cases

- [ ] All axes locked (equivalent to Fixed)
- [ ] All axes free (equivalent to Point)
- [ ] Conflicting motor+spring on same axis
- [ ] Very stiff springs

## 📚 Code Examples

### Example 1: Ball Joint (Unrestricted Rotation)

```cpp
// Create ball joint (like Unity's ConfigurableJoint)
void CreateBallJoint(Engine::Core &core, glm::vec3 position) {
    // Body A (static anchor)
    auto anchor = CreateStatic(core, position);
    
    // Body B (dynamic ball)
    auto ball = CreateDynamic(core, position + glm::vec3(0, -1, 0), 2.0f);
    
    // 6DOF configured as ball joint
    auto constraint = Physics::Component::SixDofConstraint::CreateBallJoint(
        anchor,
        ball,
        glm::vec3(0, -0.5f, 0),  // Bottom of anchor
        glm::vec3(0, 0.5f, 0)    // Top of ball
    );
    
    anchor.AddComponent<Physics::Component::SixDofConstraint>(core, constraint);
    
    Logger::Info("Ball joint created (free rotation)");
}
```

### Example 2: Ragdoll Shoulder (Limited Ball Joint)

```cpp
// Create realistic shoulder joint (ragdoll)
void CreateShoulderJoint(Engine::Core &core, Engine::Entity torso, Engine::Entity upperArm) {
    // Ragdoll joint with realistic limits
    auto shoulder = Physics::Component::SixDofConstraint::CreateRagdollJoint(
        torso,
        upperArm,
        glm::vec3(0.5f, 0.3f, 0),   // Shoulder position on torso
        glm::vec3(0, 0.3f, 0),      // Top of upper arm
        glm::radians(120.0f)        // 120° maximum angle
    );
    
    // Customize per-axis limits (more natural)
    shoulder.rotationX = Physics::Component::AxisConfig::Limited(
        glm::radians(-45.0f),   // Forward: -45°
        glm::radians(180.0f)    // Backward: 180°
    );
    shoulder.rotationY = Physics::Component::AxisConfig::Limited(
        glm::radians(-30.0f),   // Down: -30°
        glm::radians(170.0f)    // Up: 170°
    );
    shoulder.rotationZ = Physics::Component::AxisConfig::Limited(
        glm::radians(-90.0f),   // Twist left: -90°
        glm::radians(90.0f)     // Twist right: 90°
    );
    
    // Add soft constraint (more natural motion)
    shoulder.settings = Physics::Component::ConstraintSettings::Soft(0.9f, 0.2f);
    
    torso.AddComponent<Physics::Component::SixDofConstraint>(core, shoulder);
    
    Logger::Info("Shoulder joint created (ragdoll)");
}
```

### Example 3: Vehicle Suspension (Spring + Limits)

```cpp
// Create vehicle wheel suspension
void CreateWheelSuspension(Engine::Core &core, Engine::Entity chassis, Engine::Entity wheel) {
    // Suspension: vertical spring, horizontal locked
    auto suspension = Physics::Component::SixDofConstraint::CreateSuspension(
        chassis,
        wheel,
        glm::vec3(1, 0, 0),      // Wheel position on chassis
        glm::vec3(0, 0.3f, 0),   // Top of wheel
        0.8f,                    // Spring stiffness
        0.5f,                    // Damping
        0.3f                     // Suspension travel (30cm)
    );
    
    chassis.AddComponent<Physics::Component::SixDofConstraint>(core, suspension);
    
    Logger::Info("Wheel suspension created (30cm travel)");
}
```

### Example 4: Custom Constraint (Mixed Axes)

```cpp
// Create custom constraint with specific axis configuration
void CreateCustomConstraint(Engine::Core &core, glm::vec3 position) {
    auto bodyA = CreateStatic(core, position);
    auto bodyB = CreateDynamic(core, position + glm::vec3(1, 0, 0), 5.0f);
    
    auto constraint = Physics::Component::SixDofConstraint::Create(bodyA, bodyB);
    
    // Translation: X free, Y limited, Z locked
    constraint.translationX = Physics::Component::AxisConfig::Free();
    constraint.translationY = Physics::Component::AxisConfig::Limited(-2.0f, 2.0f);
    constraint.translationZ = Physics::Component::AxisConfig::Locked();
    
    // Rotation: X locked, Y free, Z limited
    constraint.rotationX = Physics::Component::AxisConfig::Locked();
    constraint.rotationY = Physics::Component::AxisConfig::Free();
    constraint.rotationZ = Physics::Component::AxisConfig::Limited(
        glm::radians(-45.0f),
        glm::radians(45.0f)
    );
    
    bodyA.AddComponent<Physics::Component::SixDofConstraint>(core, constraint);
    
    Logger::Info("Custom constraint created");
}
```

### Example 5: Motorized 6DOF (Per-Axis Control)

```cpp
// Create 6DOF with motors on multiple axes
void CreateMotorizedJoint(Engine::Core &core, glm::vec3 position) {
    auto base = CreateStatic(core, position);
    auto arm = CreateDynamic(core, position + glm::vec3(0, 1, 0), 3.0f);
    
    auto constraint = Physics::Component::SixDofConstraint::Create(base, arm);
    
    // Lock all translations
    constraint.translationX = Physics::Component::AxisConfig::Locked();
    constraint.translationY = Physics::Component::AxisConfig::Locked();
    constraint.translationZ = Physics::Component::AxisConfig::Locked();
    
    // Motorized rotations
    constraint.rotationX = Physics::Component::AxisConfig::WithMotor(
        Physics::Component::AxisMotor::Velocity(1.0f, 50.0f)  // Roll
    );
    constraint.rotationY = Physics::Component::AxisConfig::WithMotor(
        Physics::Component::AxisMotor::Position(glm::radians(45.0f), 100.0f)  // Pitch to 45°
    );
    constraint.rotationZ = Physics::Component::AxisConfig::Free();  // Free yaw
    
    base.AddComponent<Physics::Component::SixDofConstraint>(core, constraint);
    
    Logger::Info("Motorized joint created (roll + pitch motors)");
}

// Control motors at runtime
void SetJointAngles(Engine::Core &core, Engine::Entity baseEntity,
                   float roll, float pitch, float yaw) {
    auto &constraint = baseEntity.GetComponent<Physics::Component::SixDofConstraint>(core);
    
    constraint.SetAxisMotorPosition(3, roll);   // Rotation X
    constraint.SetAxisMotorPosition(4, pitch);  // Rotation Y
    constraint.SetAxisMotorPosition(5, yaw);    // Rotation Z
}
```

### Example 6: Spring-Damped Joint (Soft Constraint)

```cpp
// Create joint with springs on all axes
void CreateSpringJoint(Engine::Core &core, glm::vec3 position) {
    auto bodyA = CreateStatic(core, position);
    auto bodyB = CreateDynamic(core, position + glm::vec3(0, -2, 0), 4.0f);
    
    auto constraint = Physics::Component::SixDofConstraint::Create(bodyA, bodyB);
    
    // All translations with springs (soft tether)
    constraint.translationX = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.5f, 0.3f, 0.0f)
    );
    constraint.translationY = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.7f, 0.4f, -2.0f)  // Target 2m below
    );
    constraint.translationZ = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.5f, 0.3f, 0.0f)
    );
    
    // All rotations with springs (returns to neutral)
    constraint.rotationX = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.8f, 0.2f, 0.0f)
    );
    constraint.rotationY = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.8f, 0.2f, 0.0f)
    );
    constraint.rotationZ = Physics::Component::AxisConfig::WithSpring(
        Physics::Component::AxisSpring::Create(0.8f, 0.2f, 0.0f)
    );
    
    bodyA.AddComponent<Physics::Component::SixDofConstraint>(core, constraint);
    
    Logger::Info("Spring joint created (soft 6DOF)");
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid axis limits**
   ```cpp
   if (maxLimit < minLimit) {
       Logger::Error("6DOF: maxLimit < minLimit on axis {}", axis);
       return;
   }
   ```

2. **Conflicting motor and spring**
   ```cpp
   if (motor.enabled && spring.enabled) {
       Logger::Warning("6DOF: Motor and spring both enabled on axis {} - motor takes priority", axis);
   }
   ```

3. **Locked axis with motor**
   ```cpp
   if (motion == AxisMotion::Locked && motor.enabled) {
       Logger::Warning("6DOF: Motor enabled on locked axis {} - motor disabled", axis);
       motor.enabled = false;
   }
   ```

4. **Zero motor force**
   ```cpp
   if (motor.enabled && motor.maxForce <= 0.0f) {
       Logger::Warning("6DOF: Motor on axis {} has zero force", axis);
   }
   ```

## 💡 Design Considerations

### 6DOF vs Other Constraints

| Feature | Fixed | Distance | Point | Hinge | Slider | 6DOF |
|---------|-------|----------|-------|-------|--------|------|
| **Complexity** | Simple | Simple | Simple | Medium | Medium | Complex |
| **Performance** | Fast | Fast | Fast | Fast | Fast | Slower |
| **Flexibility** | None | None | None | Low | Low | **Maximum** |
| **Use Case** | Weld | Rope | Ball | Door | Piston | **Universal** |

### Axis Configuration Matrix

| Motion Type | Min/Max | Motor | Spring | Use Case |
|-------------|---------|-------|--------|----------|
| **Locked** | N/A | ❌ | ❌ | Fixed axis |
| **Limited** | ✅ | ✅ | ✅ | Restricted motion |
| **Free** | ❌ | ✅ | ✅ | Unlimited motion |

### Performance Notes

- **6DOF is most expensive constraint** (checks all 6 axes)
- Use simpler constraints when possible (Fixed, Hinge, etc.)
- Locked axes reduce overhead
- Motors add ~10% overhead per axis
- Springs add ~5% overhead per axis

## 📊 Success Criteria

- [ ] SixDofConstraint component implemented
- [ ] Per-axis configuration (Locked/Limited/Free)
- [ ] Per-axis limits (min/max)
- [ ] Per-axis motors (velocity + position)
- [ ] Per-axis springs (stiffness + damping)
- [ ] Factory methods (ball joint, ragdoll, suspension)
- [ ] ConstraintFactory updated
- [ ] ConstraintSystem updated
- [ ] Jolt integration working
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 50 constraints)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt 6DOF: https://jrouwe.github.io/JoltPhysics/class_six_d_o_f_constraint.html
- Unity ConfigurableJoint: https://docs.unity3d.com/Manual/class-ConfigurableJoint.html
- Godot Generic6DOFJoint: https://docs.godotengine.org/en/stable/classes/class_generic6dofjoint3d.html
- 6DOF Theory: https://www.euclideanspace.com/physics/dynamics/collision/sixdof/index.htm

## 🚀 Follow-up Issues

- #020: Debug Visualization (draw constraint axes + limits)
- #024: Constraint Solver Tuning (motor PID, spring tuning)
- #025: Soft Body Physics (use 6DOF springs)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.6`, `priority:medium`, `constraints`, `6dof`  
**Milestone:** v0.6 - Collision Events & Constraints
