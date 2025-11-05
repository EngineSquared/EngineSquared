# Issue #016: Implement Hinge & Slider Constraints (with Motors)

**Milestone:** v0.6 - Collision Events & Constraints  
**Priority:** 🟢 MEDIUM  
**Estimated Effort:** 4-5 days  
**Dependencies:** #015 (Basic Constraints)  
**Related Issues:** #017 (6DOF), #023 (Performance Profiling)

---

## 📋 Description

Implement advanced physics constraints with motor support: Hinge (revolute joint) and Slider (prismatic joint). These constraints enable mechanical systems like doors, pistons, vehicles, and articulated structures.

This issue adds constraints with single-axis motion and optional motor control.

## 🎯 Goals

- ✅ Implement HingeConstraint (rotation around axis)
- ✅ Implement SliderConstraint (translation along axis)
- ✅ Add angular/linear limits
- ✅ Add motor support (velocity/position control)
- ✅ Add servo mode (position target)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── HingeConstraint.hpp         # New file
│   └── SliderConstraint.hpp        # New file
├── resource/
│   ├── ConstraintFactory.hpp       # Modify (add Hinge/Slider)
│   └── ConstraintFactory.cpp       # Modify
├── system/
│   ├── ConstraintSystem.hpp        # Modify
│   └── ConstraintSystem.cpp        # Modify
└── tests/
    ├── HingeConstraintTest.cpp     # New file
    └── SliderConstraintTest.cpp    # New file
```

### HingeConstraint Component

```cpp
/**************************************************************************
 * @file HingeConstraint.hpp
 * @brief Hinge constraint (revolute joint - rotation around axis)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Hinge motor settings
 * 
 * Controls motor behavior (velocity or position mode).
 */
struct HingeMotor {
    enum class Mode {
        None,           ///< No motor (passive joint)
        Velocity,       ///< Velocity control (set angular velocity)
        Position        ///< Position control (servo - target angle)
    };
    
    Mode mode = Mode::None;     ///< Motor mode
    float targetVelocity = 0.0f; ///< Target angular velocity (rad/s)
    float targetAngle = 0.0f;    ///< Target angle (radians, for Position mode)
    float maxTorque = 0.0f;      ///< Maximum motor torque (Newton-meters)
    
    /**
     * @brief Create velocity motor
     * @param velocity Target angular velocity (rad/s)
     * @param torque Max torque
     * @return HingeMotor
     */
    static HingeMotor Velocity(float velocity, float torque) {
        return {Mode::Velocity, velocity, 0.0f, torque};
    }
    
    /**
     * @brief Create position servo
     * @param angle Target angle (radians)
     * @param torque Max torque
     * @return HingeMotor
     */
    static HingeMotor Position(float angle, float torque) {
        return {Mode::Position, 0.0f, angle, torque};
    }
    
    /**
     * @brief Disable motor
     * @return HingeMotor
     */
    static HingeMotor None() {
        return {Mode::None, 0.0f, 0.0f, 0.0f};
    }
};

/**
 * @brief Hinge constraint (revolute joint)
 * 
 * Allows rotation around a single axis. Bodies can rotate freely
 * (or within limits) but cannot translate or rotate on other axes.
 * 
 * **Common uses:**
 * - Doors, gates
 * - Vehicle wheels (rotation)
 * - Pendulum, swing
 * - Mechanical joints (elbow, knee with limits)
 * - Turrets, rotating platforms
 * 
 * **Physics:**
 * - 5 constraints (position x,y,z + 2 rotations)
 * - 1 degree of freedom (rotation around axis)
 * - Optional angle limits
 * - Optional motor
 * 
 * @example
 * @code
 * // Door hinge
 * auto doorFrame = CreateStatic(core, glm::vec3(0, 1, 0));
 * auto door = CreateDynamic(core, glm::vec3(1, 1, 0), 20.0f);
 * 
 * Physics::Component::HingeConstraint hinge;
 * hinge.bodyA = doorFrame;
 * hinge.bodyB = door;
 * hinge.pointA = glm::vec3(0.5f, 0, 0);  // Right edge of frame
 * hinge.pointB = glm::vec3(-0.5f, 0, 0); // Left edge of door
 * hinge.axisA = glm::vec3(0, 1, 0);      // Vertical axis
 * hinge.axisB = glm::vec3(0, 1, 0);
 * hinge.limitsEnabled = true;
 * hinge.minAngle = 0.0f;                 // 0° (closed)
 * hinge.maxAngle = glm::radians(90.0f);  // 90° (open)
 * 
 * doorFrame.AddComponent<Physics::Component::HingeConstraint>(core, hinge);
 * @endcode
 */
struct HingeConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /// Rotation axes (local space, normalized)
    glm::vec3 axisA = glm::vec3(1, 0, 0);  ///< Rotation axis on bodyA
    glm::vec3 axisB = glm::vec3(1, 0, 0);  ///< Rotation axis on bodyB
    
    /// Angle limits
    bool limitsEnabled = false;   ///< Enable angle limits
    float minAngle = 0.0f;        ///< Minimum angle (radians)
    float maxAngle = 0.0f;        ///< Maximum angle (radians)
    
    /// Motor
    HingeMotor motor = HingeMotor::None();
    
    /// Current state (read-only, updated by system)
    float currentAngle = 0.0f;    ///< Current angle (radians)
    float currentVelocity = 0.0f; ///< Current angular velocity (rad/s)
    
    /**
     * @brief Create hinge constraint
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param axisOnA Rotation axis on A (local, normalized)
     * @param axisOnB Rotation axis on B (local, normalized)
     * @param settings Constraint settings
     * @return HingeConstraint
     */
    static HingeConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA = glm::vec3(0, 1, 0),
        const glm::vec3 &axisOnB = glm::vec3(0, 1, 0),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        HingeConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.axisA = glm::normalize(axisOnA);
        c.axisB = glm::normalize(axisOnB);
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create hinge with angle limits
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param axisOnA Rotation axis on A
     * @param minAngleRad Minimum angle (radians)
     * @param maxAngleRad Maximum angle (radians)
     * @return HingeConstraint
     */
    static HingeConstraint CreateWithLimits(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA,
        float minAngleRad,
        float maxAngleRad
    ) {
        HingeConstraint c = Create(a, b, pointOnA, pointOnB, axisOnA, axisOnA);
        c.limitsEnabled = true;
        c.minAngle = minAngleRad;
        c.maxAngle = maxAngleRad;
        return c;
    }
    
    /**
     * @brief Create hinge with motor
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param axisOnA Rotation axis on A
     * @param motorSettings Motor settings
     * @return HingeConstraint
     */
    static HingeConstraint CreateWithMotor(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA,
        const HingeMotor &motorSettings
    ) {
        HingeConstraint c = Create(a, b, pointOnA, pointOnB, axisOnA, axisOnA);
        c.motor = motorSettings;
        return c;
    }
    
    /**
     * @brief Set motor velocity
     * @param velocity Angular velocity (rad/s)
     */
    void SetMotorVelocity(float velocity) {
        motor.mode = HingeMotor::Mode::Velocity;
        motor.targetVelocity = velocity;
    }
    
    /**
     * @brief Set motor target angle (servo mode)
     * @param angle Target angle (radians)
     */
    void SetMotorAngle(float angle) {
        motor.mode = HingeMotor::Mode::Position;
        motor.targetAngle = angle;
    }
    
    /**
     * @brief Disable motor
     */
    void DisableMotor() {
        motor.mode = HingeMotor::Mode::None;
    }
};

} // namespace Physics::Component
```

### SliderConstraint Component

```cpp
/**************************************************************************
 * @file SliderConstraint.hpp
 * @brief Slider constraint (prismatic joint - translation along axis)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Slider motor settings
 * 
 * Controls motor behavior (velocity or position mode).
 */
struct SliderMotor {
    enum class Mode {
        None,           ///< No motor (passive joint)
        Velocity,       ///< Velocity control (set linear velocity)
        Position        ///< Position control (servo - target position)
    };
    
    Mode mode = Mode::None;       ///< Motor mode
    float targetVelocity = 0.0f;  ///< Target linear velocity (m/s)
    float targetPosition = 0.0f;  ///< Target position (meters, for Position mode)
    float maxForce = 0.0f;        ///< Maximum motor force (Newtons)
    
    /**
     * @brief Create velocity motor
     * @param velocity Target linear velocity (m/s)
     * @param force Max force
     * @return SliderMotor
     */
    static SliderMotor Velocity(float velocity, float force) {
        return {Mode::Velocity, velocity, 0.0f, force};
    }
    
    /**
     * @brief Create position servo
     * @param position Target position (meters)
     * @param force Max force
     * @return SliderMotor
     */
    static SliderMotor Position(float position, float force) {
        return {Mode::Position, 0.0f, position, force};
    }
    
    /**
     * @brief Disable motor
     * @return SliderMotor
     */
    static SliderMotor None() {
        return {Mode::None, 0.0f, 0.0f, 0.0f};
    }
};

/**
 * @brief Slider constraint (prismatic joint)
 * 
 * Allows translation along a single axis. Bodies can slide along
 * the axis but cannot translate on other axes or rotate.
 * 
 * **Common uses:**
 * - Pistons, hydraulic arms
 * - Sliding doors
 * - Elevator platforms
 * - Telescoping objects
 * - Linear actuators
 * 
 * **Physics:**
 * - 5 constraints (2 positions + 3 rotations)
 * - 1 degree of freedom (translation along axis)
 * - Optional distance limits
 * - Optional motor
 * 
 * @example
 * @code
 * // Sliding door
 * auto doorFrame = CreateStatic(core, glm::vec3(0, 1, 0));
 * auto door = CreateDynamic(core, glm::vec3(0, 1, 0), 50.0f);
 * 
 * Physics::Component::SliderConstraint slider;
 * slider.bodyA = doorFrame;
 * slider.bodyB = door;
 * slider.pointA = glm::vec3(0, 0, 0);
 * slider.pointB = glm::vec3(0, 0, 0);
 * slider.axisA = glm::vec3(1, 0, 0);      // Slide horizontally (X axis)
 * slider.axisB = glm::vec3(1, 0, 0);
 * slider.limitsEnabled = true;
 * slider.minDistance = 0.0f;              // Closed position
 * slider.maxDistance = 2.0f;              // Open position (2m)
 * 
 * doorFrame.AddComponent<Physics::Component::SliderConstraint>(core, slider);
 * @endcode
 */
struct SliderConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /// Slide axes (local space, normalized)
    glm::vec3 axisA = glm::vec3(1, 0, 0);  ///< Slide axis on bodyA
    glm::vec3 axisB = glm::vec3(1, 0, 0);  ///< Slide axis on bodyB
    
    /// Distance limits
    bool limitsEnabled = false;   ///< Enable distance limits
    float minDistance = 0.0f;     ///< Minimum distance (meters)
    float maxDistance = 0.0f;     ///< Maximum distance (meters)
    
    /// Motor
    SliderMotor motor = SliderMotor::None();
    
    /// Current state (read-only, updated by system)
    float currentDistance = 0.0f;  ///< Current distance (meters)
    float currentVelocity = 0.0f;  ///< Current linear velocity (m/s)
    
    /**
     * @brief Create slider constraint
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param axisOnA Slide axis on A (local, normalized)
     * @param axisOnB Slide axis on B (local, normalized)
     * @param settings Constraint settings
     * @return SliderConstraint
     */
    static SliderConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA = glm::vec3(1, 0, 0),
        const glm::vec3 &axisOnB = glm::vec3(1, 0, 0),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        SliderConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.axisA = glm::normalize(axisOnA);
        c.axisB = glm::normalize(axisOnB);
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create slider with distance limits
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param axisOnA Slide axis on A
     * @param minDist Minimum distance (meters)
     * @param maxDist Maximum distance (meters)
     * @return SliderConstraint
     */
    static SliderConstraint CreateWithLimits(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA,
        float minDist,
        float maxDist
    ) {
        SliderConstraint c = Create(a, b, pointOnA, pointOnB, axisOnA, axisOnA);
        c.limitsEnabled = true;
        c.minDistance = minDist;
        c.maxDistance = maxDist;
        return c;
    }
    
    /**
     * @brief Create slider with motor
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A
     * @param pointOnB Attachment point on B
     * @param axisOnA Slide axis on A
     * @param motorSettings Motor settings
     * @return SliderConstraint
     */
    static SliderConstraint CreateWithMotor(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA,
        const glm::vec3 &pointOnB,
        const glm::vec3 &axisOnA,
        const SliderMotor &motorSettings
    ) {
        SliderConstraint c = Create(a, b, pointOnA, pointOnB, axisOnA, axisOnA);
        c.motor = motorSettings;
        return c;
    }
    
    /**
     * @brief Set motor velocity
     * @param velocity Linear velocity (m/s)
     */
    void SetMotorVelocity(float velocity) {
        motor.mode = SliderMotor::Mode::Velocity;
        motor.targetVelocity = velocity;
    }
    
    /**
     * @brief Set motor target position (servo mode)
     * @param position Target position (meters)
     */
    void SetMotorPosition(float position) {
        motor.mode = SliderMotor::Mode::Position;
        motor.targetPosition = position;
    }
    
    /**
     * @brief Disable motor
     */
    void DisableMotor() {
        motor.mode = SliderMotor::Mode::None;
    }
};

} // namespace Physics::Component
```

### ConstraintFactory Update (Add Hinge/Slider)

```cpp
// Add to ConstraintFactory.hpp

namespace Physics::Resource {

class ConstraintFactory {
public:
    // ... existing methods ...
    
    /**
     * @brief Create Jolt hinge constraint
     * @param core Engine core
     * @param constraint HingeConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreateHinge(Engine::Core &core,
                                 const Component::HingeConstraint &constraint);
    
    /**
     * @brief Create Jolt slider constraint
     * @param core Engine core
     * @param constraint SliderConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreateSlider(Engine::Core &core,
                                  const Component::SliderConstraint &constraint);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create Hinge Component (3-4 hours)

- [ ] Create `component/HingeConstraint.hpp`
- [ ] Implement HingeMotor struct (None/Velocity/Position)
- [ ] Implement HingeConstraint struct
- [ ] Add angle limits (min/max)
- [ ] Add motor control
- [ ] Add factory methods (Create/CreateWithLimits/CreateWithMotor)
- [ ] Add SetMotorVelocity/SetMotorAngle methods
- [ ] Add Doxygen comments with examples
- [ ] Add to `Physics.hpp`

### Phase 2: Create Slider Component (3-4 hours)

- [ ] Create `component/SliderConstraint.hpp`
- [ ] Implement SliderMotor struct (None/Velocity/Position)
- [ ] Implement SliderConstraint struct
- [ ] Add distance limits (min/max)
- [ ] Add motor control
- [ ] Add factory methods
- [ ] Add SetMotorVelocity/SetMotorPosition methods
- [ ] Add Doxygen comments with examples
- [ ] Add to `Physics.hpp`

### Phase 3: Implement Hinge Factory (8-10 hours)

- [ ] Update `resource/ConstraintFactory.hpp`
- [ ] Implement CreateHinge()
  - Create JPH::HingeConstraintSettings
  - Set attachment point and axis
  - Configure angle limits (if enabled)
  - Configure motor (velocity/position/none)
  - Add to Jolt world
- [ ] Handle motor mode switching
- [ ] Update current angle/velocity

### Phase 4: Implement Slider Factory (8-10 hours)

- [ ] Update `resource/ConstraintFactory.hpp`
- [ ] Implement CreateSlider()
  - Create JPH::SliderConstraintSettings
  - Set attachment point and axis
  - Configure distance limits (if enabled)
  - Configure motor (velocity/position/none)
  - Add to Jolt world
- [ ] Handle motor mode switching
- [ ] Update current distance/velocity

### Phase 5: Update ConstraintSystem (4-5 hours)

- [ ] Update `system/ConstraintSystem.hpp`
- [ ] Add ProcessNewHingeConstraints()
- [ ] Add ProcessNewSliderConstraints()
- [ ] Update motor settings on change
- [ ] Update current state (angle/distance/velocity)
- [ ] Handle motor force/torque clamping

### Phase 6: Unit Tests (10-12 hours)

- [ ] Create `tests/HingeConstraintTest.cpp`
- [ ] Test hinge rotation (free rotation)
- [ ] Test angle limits (min/max)
- [ ] Test velocity motor
- [ ] Test position servo
- [ ] Test motor force clamping
- [ ] Create `tests/SliderConstraintTest.cpp`
- [ ] Test slider translation
- [ ] Test distance limits
- [ ] Test velocity motor
- [ ] Test position servo

### Phase 7: Integration Examples (8-10 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Door Hinge" example (limited rotation)
- [ ] Add "Motorized Door" example (servo opens/closes)
- [ ] Add "Wheel" example (continuous rotation motor)
- [ ] Add "Sliding Door" example (slider with limits)
- [ ] Add "Piston" example (slider with motor)
- [ ] Add "Elevator" example (slider position control)

### Phase 8: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Hinge Constraints" section
  - Add "Slider Constraints" section
  - Explain motor modes (velocity vs position)
  - Explain limits
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Hinge allows rotation around axis only
- [ ] Hinge respects angle limits
- [ ] Hinge velocity motor works
- [ ] Hinge position servo reaches target
- [ ] Slider allows translation along axis only
- [ ] Slider respects distance limits
- [ ] Slider velocity motor works
- [ ] Slider position servo reaches target
- [ ] Motor force/torque clamping works
- [ ] Disabling motor stops motion

### Integration Tests

- [ ] Door opens/closes with hinge
- [ ] Motorized door moves to target angle
- [ ] Wheel spins continuously
- [ ] Sliding door moves along track
- [ ] Piston extends/retracts
- [ ] Elevator moves to target floor

### Performance Tests

- [ ] 50 hinges: < 3ms overhead
- [ ] 50 sliders: < 3ms overhead
- [ ] Motor control: < 0.5ms per constraint

### Edge Cases

- [ ] Limits with motor (motor pushes against limit)
- [ ] Zero max force/torque
- [ ] Switching motor modes at runtime
- [ ] Very fast motor velocities

## 📚 Code Examples

### Example 1: Door Hinge (Limited Rotation)

```cpp
// Create door that swings open (0° to 90°)
void CreateDoor(Engine::Core &core, glm::vec3 framePos) {
    // Door frame (static)
    auto frame = core.CreateEntity();
    frame.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(framePos));
    
    Physics::Component::RigidBody rbFrame = Physics::Component::RigidBody::CreateStatic();
    frame.AddComponent<Physics::Component::RigidBody>(core, rbFrame);
    
    Physics::Component::BoxCollider colliderFrame(glm::vec3(0.1f, 2.0f, 1.0f));
    frame.AddComponent<Physics::Component::BoxCollider>(core, colliderFrame);
    
    // Door (dynamic)
    auto door = core.CreateEntity();
    door.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(framePos + glm::vec3(1.0f, 0, 0)));
    
    Physics::Component::RigidBody rbDoor = Physics::Component::RigidBody::CreateDynamic(20.0f);
    door.AddComponent<Physics::Component::RigidBody>(core, rbDoor);
    
    Physics::Component::BoxCollider colliderDoor(glm::vec3(1.0f, 2.0f, 0.05f));
    door.AddComponent<Physics::Component::BoxCollider>(core, colliderDoor);
    
    // Hinge constraint (vertical axis, 0° to 90°)
    auto hinge = Physics::Component::HingeConstraint::CreateWithLimits(
        frame,
        door,
        glm::vec3(0.1f, 0, 0),   // Right edge of frame
        glm::vec3(-1.0f, 0, 0),  // Left edge of door
        glm::vec3(0, 1, 0),      // Vertical axis (Y)
        0.0f,                    // Min angle: 0° (closed)
        glm::radians(90.0f)      // Max angle: 90° (open)
    );
    
    frame.AddComponent<Physics::Component::HingeConstraint>(core, hinge);
    
    Logger::Info("Door created (swings 0° to 90°)");
}
```

### Example 2: Motorized Door (Servo Control)

```cpp
// Create door with motor (opens/closes automatically)
void CreateMotorizedDoor(Engine::Core &core, glm::vec3 framePos) {
    // Frame + door (same as Example 1)
    auto frame = CreateDoorFrame(core, framePos);
    auto door = CreateDoorBody(core, framePos + glm::vec3(1.0f, 0, 0), 20.0f);
    
    // Hinge with position servo motor
    auto hinge = Physics::Component::HingeConstraint::CreateWithMotor(
        frame,
        door,
        glm::vec3(0.1f, 0, 0),
        glm::vec3(-1.0f, 0, 0),
        glm::vec3(0, 1, 0),
        Physics::Component::HingeMotor::Position(glm::radians(90.0f), 50.0f)  // Open to 90°
    );
    
    // Set limits too
    hinge.limitsEnabled = true;
    hinge.minAngle = 0.0f;
    hinge.maxAngle = glm::radians(90.0f);
    
    frame.AddComponent<Physics::Component::HingeConstraint>(core, hinge);
    
    Logger::Info("Motorized door created (auto-opens to 90°)");
}

// Open/close door at runtime
void ToggleDoor(Engine::Core &core, Engine::Entity frameEntity, bool open) {
    auto &hinge = frameEntity.GetComponent<Physics::Component::HingeConstraint>(core);
    
    if (open) {
        hinge.SetMotorAngle(glm::radians(90.0f));  // Open
        Logger::Info("Door opening...");
    } else {
        hinge.SetMotorAngle(0.0f);  // Close
        Logger::Info("Door closing...");
    }
}
```

### Example 3: Spinning Wheel (Continuous Rotation)

```cpp
// Create wheel that spins continuously (vehicle wheel)
void CreateWheel(Engine::Core &core, glm::vec3 axlePos) {
    // Axle (static or kinematic for vehicle chassis)
    auto axle = core.CreateEntity();
    axle.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(axlePos));
    
    Physics::Component::RigidBody rbAxle = Physics::Component::RigidBody::CreateStatic();
    axle.AddComponent<Physics::Component::RigidBody>(core, rbAxle);
    
    // Wheel (dynamic cylinder)
    auto wheel = core.CreateEntity();
    wheel.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(axlePos));
    
    Physics::Component::RigidBody rbWheel = Physics::Component::RigidBody::CreateDynamic(5.0f);
    wheel.AddComponent<Physics::Component::RigidBody>(core, rbWheel);
    
    Physics::Component::CylinderCollider collider(0.3f, 0.1f);  // Radius 0.3m, thickness 0.1m
    wheel.AddComponent<Physics::Component::CylinderCollider>(core, collider);
    
    // Hinge with velocity motor (spins continuously)
    auto hinge = Physics::Component::HingeConstraint::CreateWithMotor(
        axle,
        wheel,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1),      // Axis: forward (Z)
        Physics::Component::HingeMotor::Velocity(10.0f, 100.0f)  // 10 rad/s, 100 Nm torque
    );
    
    axle.AddComponent<Physics::Component::HingeConstraint>(core, hinge);
    
    Logger::Info("Wheel created (spinning at 10 rad/s)");
}

// Accelerate/brake wheel
void SetWheelSpeed(Engine::Core &core, Engine::Entity axleEntity, float speed) {
    auto &hinge = axleEntity.GetComponent<Physics::Component::HingeConstraint>(core);
    hinge.SetMotorVelocity(speed);
}
```

### Example 4: Sliding Door (Horizontal Track)

```cpp
// Create sliding door (moves left/right on track)
void CreateSlidingDoor(Engine::Core &core, glm::vec3 trackPos) {
    // Track (static)
    auto track = core.CreateEntity();
    track.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(trackPos));
    
    Physics::Component::RigidBody rbTrack = Physics::Component::RigidBody::CreateStatic();
    track.AddComponent<Physics::Component::RigidBody>(core, rbTrack);
    
    // Door (dynamic)
    auto door = core.CreateEntity();
    door.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(trackPos));
    
    Physics::Component::RigidBody rbDoor = Physics::Component::RigidBody::CreateDynamic(50.0f);
    door.AddComponent<Physics::Component::RigidBody>(core, rbDoor);
    
    Physics::Component::BoxCollider colliderDoor(glm::vec3(1.0f, 2.0f, 0.05f));
    door.AddComponent<Physics::Component::BoxCollider>(core, colliderDoor);
    
    // Slider constraint (horizontal axis, 0m to 2m)
    auto slider = Physics::Component::SliderConstraint::CreateWithLimits(
        track,
        door,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(1, 0, 0),  // Slide along X axis
        0.0f,                // Closed position
        2.0f                 // Open position (2m)
    );
    
    track.AddComponent<Physics::Component::SliderConstraint>(core, slider);
    
    Logger::Info("Sliding door created (0m to 2m)");
}
```

### Example 5: Piston (Linear Actuator with Motor)

```cpp
// Create piston that extends/retracts
void CreatePiston(Engine::Core &core, glm::vec3 basePos) {
    // Piston base (static)
    auto base = core.CreateEntity();
    base.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(basePos));
    
    Physics::Component::RigidBody rbBase = Physics::Component::RigidBody::CreateStatic();
    base.AddComponent<Physics::Component::RigidBody>(core, rbBase);
    
    Physics::Component::BoxCollider colliderBase(glm::vec3(0.2f, 0.2f, 0.5f));
    base.AddComponent<Physics::Component::BoxCollider>(core, colliderBase);
    
    // Piston rod (dynamic)
    auto rod = core.CreateEntity();
    rod.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(basePos + glm::vec3(0, 0, 0.5f)));
    
    Physics::Component::RigidBody rbRod = Physics::Component::RigidBody::CreateDynamic(3.0f);
    rod.AddComponent<Physics::Component::RigidBody>(core, rbRod);
    
    Physics::Component::CapsuleCollider colliderRod(1.0f, 0.1f);
    rod.AddComponent<Physics::Component::CapsuleCollider>(core, colliderRod);
    
    // Slider with position servo (extends to 1m)
    auto slider = Physics::Component::SliderConstraint::CreateWithMotor(
        base,
        rod,
        glm::vec3(0, 0, 0.5f),  // Front of base
        glm::vec3(0, 0, -0.5f), // Back of rod
        glm::vec3(0, 0, 1),     // Axis: forward (Z)
        Physics::Component::SliderMotor::Position(1.0f, 500.0f)  // Extend to 1m
    );
    
    // Set limits
    slider.limitsEnabled = true;
    slider.minDistance = 0.0f;   // Retracted
    slider.maxDistance = 1.5f;   // Fully extended
    
    base.AddComponent<Physics::Component::SliderConstraint>(core, slider);
    
    Logger::Info("Piston created (extends to 1m)");
}

// Extend/retract piston
void SetPistonExtension(Engine::Core &core, Engine::Entity baseEntity, float distance) {
    auto &slider = baseEntity.GetComponent<Physics::Component::SliderConstraint>(core);
    slider.SetMotorPosition(distance);
}
```

### Example 6: Elevator (Position Control)

```cpp
// Create elevator with multiple floors
void CreateElevator(Engine::Core &core, glm::vec3 shaftPos) {
    // Elevator shaft (static)
    auto shaft = core.CreateEntity();
    shaft.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(shaftPos));
    
    Physics::Component::RigidBody rbShaft = Physics::Component::RigidBody::CreateStatic();
    shaft.AddComponent<Physics::Component::RigidBody>(core, rbShaft);
    
    // Elevator platform (dynamic)
    auto platform = core.CreateEntity();
    platform.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(shaftPos));
    
    Physics::Component::RigidBody rbPlatform = Physics::Component::RigidBody::CreateDynamic(100.0f);
    platform.AddComponent<Physics::Component::RigidBody>(core, rbPlatform);
    
    Physics::Component::BoxCollider colliderPlatform(glm::vec3(2.0f, 0.1f, 2.0f));
    platform.AddComponent<Physics::Component::BoxCollider>(core, colliderPlatform);
    
    // Slider with position servo (vertical axis)
    auto slider = Physics::Component::SliderConstraint::CreateWithMotor(
        shaft,
        platform,
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0),      // Axis: up (Y)
        Physics::Component::SliderMotor::Position(0.0f, 1000.0f)  // Start at ground floor
    );
    
    slider.limitsEnabled = true;
    slider.minDistance = 0.0f;    // Ground floor
    slider.maxDistance = 30.0f;   // Top floor (30m)
    
    shaft.AddComponent<Physics::Component::SliderConstraint>(core, slider);
    
    Logger::Info("Elevator created (4 floors)");
}

// Move elevator to floor
void MoveElevatorToFloor(Engine::Core &core, Engine::Entity shaftEntity, int floor) {
    auto &slider = shaftEntity.GetComponent<Physics::Component::SliderConstraint>(core);
    
    const float floorHeight = 3.0f;  // 3m per floor
    float targetHeight = floor * floorHeight;
    
    slider.SetMotorPosition(targetHeight);
    
    Logger::Info("Elevator moving to floor {}", floor);
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid axis (zero length)**
   ```cpp
   if (glm::length(axis) < 0.001f) {
       Logger::Error("HingeConstraint: Axis has zero length");
       return;
   }
   ```

2. **Invalid limits**
   ```cpp
   if (maxAngle < minAngle) {
       Logger::Error("HingeConstraint: maxAngle < minAngle");
       return;
   }
   ```

3. **Zero motor force/torque**
   ```cpp
   if (motor.mode != HingeMotor::Mode::None && motor.maxTorque <= 0.0f) {
       Logger::Warning("HingeConstraint: Motor enabled but maxTorque is 0");
   }
   ```

4. **Motor target out of limits**
   ```cpp
   if (limitsEnabled && motor.targetAngle > maxAngle) {
       Logger::Warning("HingeConstraint: Target angle exceeds limit, clamping");
       motor.targetAngle = maxAngle;
   }
   ```

## 💡 Design Considerations

### Constraint Comparison

| Type | DOF | Limits | Motor | Use Case |
|------|-----|--------|-------|----------|
| **Hinge** | 1 (rotation) | Angle | Torque | Doors, wheels, joints |
| **Slider** | 1 (translation) | Distance | Force | Pistons, elevators, slides |

### Motor Modes

```cpp
// Velocity mode: Set constant speed
motor.mode = HingeMotor::Mode::Velocity;
motor.targetVelocity = 5.0f;  // 5 rad/s

// Position mode: Servo to target angle
motor.mode = HingeMotor::Mode::Position;
motor.targetAngle = glm::radians(45.0f);  // 45°
```

### Performance

- Hinges/sliders are efficient (single-axis constraints)
- Motor control adds ~10% overhead
- Position servos calculate error every frame
- Velocity motors apply constant force/torque

## 📊 Success Criteria

- [ ] HingeConstraint component implemented
- [ ] SliderConstraint component implemented
- [ ] Motor support (velocity + position)
- [ ] Angle/distance limits working
- [ ] ConstraintFactory updated
- [ ] ConstraintSystem updated
- [ ] Jolt integration working
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 3ms for 50 constraints)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Hinge: https://jrouwe.github.io/JoltPhysics/class_hinge_constraint.html
- Jolt Slider: https://jrouwe.github.io/JoltPhysics/class_slider_constraint.html
- Unity Hinge Joint: https://docs.unity3d.com/Manual/class-HingeJoint.html
- Godot Hinge Joint: https://docs.godotengine.org/en/stable/classes/class_hingejoint3d.html

## 🚀 Follow-up Issues

- #017: 6DOF Constraint (configurable all axes)
- #020: Debug Visualization (draw constraints + motors)
- #024: Constraint Solver Tuning (motor PID control)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.6`, `priority:medium`, `constraints`, `motors`  
**Milestone:** v0.6 - Collision Events & Constraints
