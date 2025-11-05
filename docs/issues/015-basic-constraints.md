# Issue #015: Implement Basic Constraints (Fixed, Distance, Point)

**Milestone:** v0.6 - Collision Events & Constraints  
**Priority:** 🟢 MEDIUM  
**Estimated Effort:** 3-4 days  
**Dependencies:** #001-012 (Core physics)  
**Related Issues:** #016 (Hinge/Slider), #017 (6DOF)

---

## 📋 Description

Implement basic physics constraints to connect rigid bodies together. Constraints restrict motion between bodies, enabling features like ragdolls, ropes, chains, vehicles, and mechanical systems.

This issue covers the three fundamental constraint types: Fixed (weld), Distance (rope), and Point (ball joint).

## 🎯 Goals

- ✅ Implement FixedConstraint (weld two bodies)
- ✅ Implement DistanceConstraint (maintain distance)
- ✅ Implement PointConstraint (ball/socket joint)
- ✅ Constraint creation/destruction API
- ✅ Constraint breaking (threshold-based)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Constraint.hpp              # New file (base)
│   ├── FixedConstraint.hpp         # New file
│   ├── DistanceConstraint.hpp      # New file
│   └── PointConstraint.hpp         # New file
├── resource/
│   ├── ConstraintFactory.hpp       # New file
│   └── ConstraintFactory.cpp       # New file
├── system/
│   ├── ConstraintSystem.hpp        # New file
│   └── ConstraintSystem.cpp        # New file
└── tests/
    ├── FixedConstraintTest.cpp     # New file
    ├── DistanceConstraintTest.cpp  # New file
    └── PointConstraintTest.cpp     # New file
```

### Base Constraint Component

```cpp
/**************************************************************************
 * @file Constraint.hpp
 * @brief Base constraint component
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <cstdint>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Base constraint settings
 * 
 * Shared settings for all constraint types.
 */
struct ConstraintSettings {
    /// Breaking force threshold (0 = unbreakable)
    float breakForce = 0.0f;   ///< Force required to break constraint (Newtons)
    float breakTorque = 0.0f;  ///< Torque required to break constraint (Newton-meters)
    
    /// Constraint strength
    float stiffness = 1.0f;    ///< How stiff the constraint is [0, 1] (1 = rigid)
    float damping = 0.0f;      ///< Damping coefficient [0, 1] (0 = no damping)
    
    /// Collision between constrained bodies
    bool enableCollision = false;  ///< Allow collision between connected bodies
    
    /**
     * @brief Check if constraint can break
     * @return true if break thresholds set
     */
    bool IsBreakable() const {
        return breakForce > 0.0f || breakTorque > 0.0f;
    }
    
    /**
     * @brief Create unbreakable constraint
     * @return ConstraintSettings
     */
    static ConstraintSettings Rigid() {
        return {0.0f, 0.0f, 1.0f, 0.0f, false};
    }
    
    /**
     * @brief Create breakable constraint
     * @param force Break force
     * @param torque Break torque
     * @return ConstraintSettings
     */
    static ConstraintSettings Breakable(float force, float torque = 0.0f) {
        return {force, torque, 1.0f, 0.0f, false};
    }
    
    /**
     * @brief Create soft constraint (spring-like)
     * @param stiff Stiffness [0, 1]
     * @param damp Damping [0, 1]
     * @return ConstraintSettings
     */
    static ConstraintSettings Soft(float stiff = 0.5f, float damp = 0.1f) {
        return {0.0f, 0.0f, stiff, damp, false};
    }
};

/**
 * @brief Base constraint data
 * 
 * Common data for all constraints.
 */
struct Constraint {
    Engine::Entity bodyA;       ///< First body (invalid = world)
    Engine::Entity bodyB;       ///< Second body
    ConstraintSettings settings; ///< Constraint settings
    bool broken = false;        ///< Whether constraint has broken
    
    /// Jolt constraint handle (internal)
    uint32_t joltConstraintId = 0;
    
    /**
     * @brief Check if constraint is between two bodies
     * @return true if both bodies valid
     */
    bool IsBiDirectional() const {
        return bodyA.IsValid() && bodyB.IsValid();
    }
    
    /**
     * @brief Check if constraint is to world
     * @return true if bodyA is invalid (world anchor)
     */
    bool IsWorldConstrained() const {
        return !bodyA.IsValid() && bodyB.IsValid();
    }
};

} // namespace Physics::Component
```

### FixedConstraint Component

```cpp
/**************************************************************************
 * @file FixedConstraint.hpp
 * @brief Fixed constraint (weld two bodies together)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Fixed constraint (weld)
 * 
 * Rigidly attaches two bodies together as if they were one.
 * No relative motion allowed between bodies.
 * 
 * **Common uses:**
 * - Attach objects permanently (glue, weld)
 * - Ragdoll bone connections (rigid bones)
 * - Vehicle chassis parts
 * - Compound objects
 * 
 * **Physics:**
 * - Zero degrees of freedom
 * - Bodies move as one unit
 * - Can be broken by force/torque
 * 
 * @example
 * @code
 * // Weld two boxes together
 * auto boxA = CreateBox(core, glm::vec3(0, 0, 0));
 * auto boxB = CreateBox(core, glm::vec3(2, 0, 0));
 * 
 * Physics::Component::FixedConstraint constraint;
 * constraint.bodyA = boxA;
 * constraint.bodyB = boxB;
 * constraint.pointA = glm::vec3(1, 0, 0);  // Right side of boxA
 * constraint.pointB = glm::vec3(-1, 0, 0); // Left side of boxB
 * 
 * boxA.AddComponent<Physics::Component::FixedConstraint>(core, constraint);
 * @endcode
 */
struct FixedConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /// Orientation alignment
    glm::quat rotationA = glm::quat(1, 0, 0, 0);  ///< Rotation of bodyA at attachment
    glm::quat rotationB = glm::quat(1, 0, 0, 0);  ///< Rotation of bodyB at attachment
    
    /**
     * @brief Create fixed constraint between two bodies
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param settings Constraint settings
     * @return FixedConstraint
     */
    static FixedConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        FixedConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create fixed constraint to world
     * @param body Body to constrain
     * @param worldPoint World position to fix to
     * @param settings Constraint settings
     * @return FixedConstraint
     */
    static FixedConstraint CreateToWorld(
        Engine::Entity body,
        const glm::vec3 &worldPoint,
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        FixedConstraint c;
        c.bodyA = Engine::Entity();  // Invalid = world
        c.bodyB = body;
        c.pointA = worldPoint;  // World space
        c.pointB = glm::vec3(0.0f);
        c.settings = settings;
        return c;
    }
};

} // namespace Physics::Component
```

### DistanceConstraint Component

```cpp
/**************************************************************************
 * @file DistanceConstraint.hpp
 * @brief Distance constraint (maintain distance between bodies)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Distance constraint (rope/spring)
 * 
 * Maintains a specific distance between two bodies.
 * Bodies can rotate freely but stay at fixed distance.
 * 
 * **Common uses:**
 * - Ropes, chains
 * - Springs, elastic bands
 * - Character tether (leash)
 * - Grappling hook
 * - Pendulum
 * 
 * **Physics:**
 * - 1 constraint (distance)
 * - 5 degrees of freedom (all but distance)
 * - Can be min/max distance or exact
 * 
 * @example
 * @code
 * // Rope between two objects
 * auto anchor = CreateBox(core, glm::vec3(0, 5, 0));  // Static anchor
 * auto weight = CreateBox(core, glm::vec3(0, 0, 0));  // Hanging weight
 * 
 * Physics::Component::DistanceConstraint constraint;
 * constraint.bodyA = anchor;
 * constraint.bodyB = weight;
 * constraint.distance = 5.0f;  // 5 meter rope
 * constraint.minDistance = 0.0f;
 * constraint.maxDistance = 5.0f;
 * 
 * anchor.AddComponent<Physics::Component::DistanceConstraint>(core, constraint);
 * @endcode
 */
struct DistanceConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /// Distance constraints
    float minDistance = 0.0f;  ///< Minimum distance (0 = no limit)
    float maxDistance = 0.0f;  ///< Maximum distance (0 = no limit)
    float distance = 0.0f;     ///< Target distance (if min == max)
    
    /**
     * @brief Create distance constraint (exact distance)
     * @param a First body
     * @param b Second body
     * @param dist Target distance
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param settings Constraint settings
     * @return DistanceConstraint
     */
    static DistanceConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        float dist,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        DistanceConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.distance = dist;
        c.minDistance = dist;
        c.maxDistance = dist;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create distance constraint with range
     * @param a First body
     * @param b Second body
     * @param minDist Minimum distance
     * @param maxDist Maximum distance
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param settings Constraint settings
     * @return DistanceConstraint
     */
    static DistanceConstraint CreateWithRange(
        Engine::Entity a,
        Engine::Entity b,
        float minDist,
        float maxDist,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f),
        const ConstraintSettings &settings = ConstraintSettings::Soft()
    ) {
        DistanceConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.minDistance = minDist;
        c.maxDistance = maxDist;
        c.distance = (minDist + maxDist) * 0.5f;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create rope constraint (max distance only)
     * @param a First body
     * @param b Second body
     * @param length Rope length
     * @return DistanceConstraint
     */
    static DistanceConstraint CreateRope(
        Engine::Entity a,
        Engine::Entity b,
        float length
    ) {
        return CreateWithRange(a, b, 0.0f, length, glm::vec3(0.0f), glm::vec3(0.0f),
                              ConstraintSettings::Soft(0.8f, 0.2f));
    }
};

} // namespace Physics::Component
```

### PointConstraint Component

```cpp
/**************************************************************************
 * @file PointConstraint.hpp
 * @brief Point constraint (ball/socket joint)
 **************************************************************************/

#pragma once

#include "Constraint.hpp"

namespace Physics::Component {

/**
 * @brief Point constraint (ball/socket joint)
 * 
 * Connects two bodies at a single point. Bodies can rotate freely
 * around the connection point but cannot translate.
 * 
 * **Common uses:**
 * - Ragdoll joints (shoulders, hips)
 * - Chain links
 * - Rope segments
 * - Swinging objects
 * - Character limbs
 * 
 * **Physics:**
 * - 3 constraints (position x, y, z)
 * - 3 degrees of freedom (rotation only)
 * - Like a ball-and-socket joint
 * 
 * @example
 * @code
 * // Ragdoll arm connection (shoulder)
 * auto torso = CreateCapsule(core, glm::vec3(0, 1, 0));
 * auto upperArm = CreateCapsule(core, glm::vec3(1, 1, 0));
 * 
 * Physics::Component::PointConstraint constraint;
 * constraint.bodyA = torso;
 * constraint.bodyB = upperArm;
 * constraint.pointA = glm::vec3(0.5f, 0.5f, 0);  // Shoulder position on torso
 * constraint.pointB = glm::vec3(-0.3f, 0.3f, 0); // Top of upper arm
 * 
 * torso.AddComponent<Physics::Component::PointConstraint>(core, constraint);
 * @endcode
 */
struct PointConstraint : public Constraint {
    /// Attachment points in local space
    glm::vec3 pointA = glm::vec3(0.0f);  ///< Point on bodyA (local space)
    glm::vec3 pointB = glm::vec3(0.0f);  ///< Point on bodyB (local space)
    
    /**
     * @brief Create point constraint between two bodies
     * @param a First body
     * @param b Second body
     * @param pointOnA Attachment point on A (local)
     * @param pointOnB Attachment point on B (local)
     * @param settings Constraint settings
     * @return PointConstraint
     */
    static PointConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointOnA = glm::vec3(0.0f),
        const glm::vec3 &pointOnB = glm::vec3(0.0f),
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        PointConstraint c;
        c.bodyA = a;
        c.bodyB = b;
        c.pointA = pointOnA;
        c.pointB = pointOnB;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Create point constraint to world
     * @param body Body to constrain
     * @param worldPoint World position to fix to
     * @param settings Constraint settings
     * @return PointConstraint
     */
    static PointConstraint CreateToWorld(
        Engine::Entity body,
        const glm::vec3 &worldPoint,
        const ConstraintSettings &settings = ConstraintSettings::Rigid()
    ) {
        PointConstraint c;
        c.bodyA = Engine::Entity();  // Invalid = world
        c.bodyB = body;
        c.pointA = worldPoint;  // World space
        c.pointB = glm::vec3(0.0f);
        c.settings = settings;
        return c;
    }
};

} // namespace Physics::Component
```

### ConstraintFactory Resource

```cpp
/**************************************************************************
 * @file ConstraintFactory.hpp
 * @brief Factory for creating Jolt constraints
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {
    struct FixedConstraint;
    struct DistanceConstraint;
    struct PointConstraint;
}

namespace Physics::Resource {

/**
 * @brief Constraint factory
 * 
 * Creates Jolt constraints from EngineSquared components.
 */
class ConstraintFactory {
public:
    /**
     * @brief Create Jolt fixed constraint
     * @param core Engine core
     * @param constraint FixedConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreateFixed(Engine::Core &core,
                                 const Component::FixedConstraint &constraint);
    
    /**
     * @brief Create Jolt distance constraint
     * @param core Engine core
     * @param constraint DistanceConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreateDistance(Engine::Core &core,
                                    const Component::DistanceConstraint &constraint);
    
    /**
     * @brief Create Jolt point constraint
     * @param core Engine core
     * @param constraint PointConstraint component
     * @return Jolt constraint ID
     */
    static uint32_t CreatePoint(Engine::Core &core,
                                 const Component::PointConstraint &constraint);
    
    /**
     * @brief Destroy Jolt constraint
     * @param core Engine core
     * @param constraintId Jolt constraint ID
     */
    static void DestroyConstraint(Engine::Core &core, uint32_t constraintId);
};

} // namespace Physics::Resource
```

### ConstraintSystem

```cpp
/**************************************************************************
 * @file ConstraintSystem.hpp
 * @brief System for managing constraints
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::System {

/**
 * @brief Constraint system
 * 
 * Manages constraint lifecycle:
 * 1. Detect new constraint components
 * 2. Create Jolt constraints
 * 3. Monitor constraint forces
 * 4. Break constraints when threshold exceeded
 * 5. Cleanup destroyed constraints
 */
class ConstraintSystem {
public:
    /**
     * @brief Initialize constraint system
     * @param core Engine core
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown constraint system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Update constraints (called after physics step)
     * @param core Engine core
     * 
     * Checks for broken constraints and removes them.
     */
    static void Update(Engine::Core &core);
    
    /**
     * @brief Process new constraints
     * @param core Engine core
     * 
     * Creates Jolt constraints for new components.
     */
    static void ProcessNewConstraints(Engine::Core &core);
    
    /**
     * @brief Remove constraint
     * @param core Engine core
     * @param entity Entity with constraint component
     */
    static void RemoveConstraint(Engine::Core &core, Engine::Entity entity);
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Create Base Constraint (2-3 hours)

- [ ] Create `component/Constraint.hpp`
- [ ] Implement ConstraintSettings struct
- [ ] Implement base Constraint struct
- [ ] Add breaking settings (force/torque thresholds)
- [ ] Add stiffness/damping settings
- [ ] Add factory methods
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create Constraint Components (3-4 hours)

- [ ] Create `component/FixedConstraint.hpp`
- [ ] Create `component/DistanceConstraint.hpp`
- [ ] Create `component/PointConstraint.hpp`
- [ ] Implement factory methods for each
- [ ] Add Doxygen comments with examples
- [ ] Add to `Physics.hpp`

### Phase 3: Implement ConstraintFactory (8-10 hours)

- [ ] Create `resource/ConstraintFactory.hpp`
- [ ] Implement CreateFixed()
  - Create JPH::FixedConstraintSettings
  - Set attachment points
  - Set stiffness/damping
  - Add to Jolt world
- [ ] Implement CreateDistance()
  - Create JPH::DistanceConstraintSettings
  - Set min/max distance
  - Configure spring parameters
- [ ] Implement CreatePoint()
  - Create JPH::PointConstraintSettings
  - Set attachment point
- [ ] Implement DestroyConstraint()
- [ ] Handle world-constrained bodies

### Phase 4: Implement ConstraintSystem (6-8 hours)

- [ ] Create `system/ConstraintSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement ProcessNewConstraints()
  - Detect new constraint components
  - Call ConstraintFactory
  - Store Jolt constraint ID
- [ ] Implement Update()
  - Check constraint forces
  - Break if threshold exceeded
  - Remove broken constraints
- [ ] Implement RemoveConstraint()
- [ ] Handle component destruction

### Phase 5: Unit Tests (8-10 hours)

- [ ] Create `tests/FixedConstraintTest.cpp`
- [ ] Test fixed constraint holds bodies together
- [ ] Test fixed constraint breaks at threshold
- [ ] Create `tests/DistanceConstraintTest.cpp`
- [ ] Test distance constraint maintains distance
- [ ] Test min/max distance range
- [ ] Create `tests/PointConstraintTest.cpp`
- [ ] Test point constraint allows rotation
- [ ] Test world-constrained bodies
- [ ] Test constraint cleanup

### Phase 6: Integration Examples (6-7 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Weld Two Objects" example (Fixed)
- [ ] Add "Rope/Chain" example (Distance)
- [ ] Add "Pendulum" example (Point to world)
- [ ] Add "Ragdoll Arm" example (Point between bodies)
- [ ] Add "Breaking Constraint" example

### Phase 7: Documentation (3-4 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Constraints" section
  - Explain constraint types
  - Breaking mechanics
  - Performance considerations
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Fixed constraint holds bodies rigidly
- [ ] Distance constraint maintains distance
- [ ] Point constraint allows rotation
- [ ] Constraints break at threshold
- [ ] World-constrained bodies work
- [ ] Constraint cleanup works
- [ ] Multiple constraints per body
- [ ] Soft constraints (spring-like)

### Integration Tests

- [ ] Welded objects move as one
- [ ] Rope hangs realistically
- [ ] Pendulum swings correctly
- [ ] Ragdoll joints flex properly
- [ ] Breaking constraints trigger correctly

### Performance Tests

- [ ] 100 constraints: < 5ms overhead
- [ ] 1000 constraints: < 50ms overhead
- [ ] Constraint breaking check: < 1ms

### Edge Cases

- [ ] Constraint between deleted entities
- [ ] Self-constraint (body to itself)
- [ ] Overlapping attachment points
- [ ] Very stiff/soft constraints

## 📚 Code Examples

### Example 1: Weld Two Boxes (Fixed Constraint)

```cpp
// Create two boxes and weld them together
void CreateWeldedBoxes(Engine::Core &core) {
    // Box A (dynamic)
    auto boxA = core.CreateEntity();
    boxA.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(glm::vec3(0, 2, 0)));
    
    Physics::Component::RigidBody rbA = Physics::Component::RigidBody::CreateDynamic(10.0f);
    boxA.AddComponent<Physics::Component::RigidBody>(core, rbA);
    
    Physics::Component::BoxCollider colliderA(glm::vec3(0.5f));
    boxA.AddComponent<Physics::Component::BoxCollider>(core, colliderA);
    
    // Box B (dynamic)
    auto boxB = core.CreateEntity();
    boxB.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(glm::vec3(1.5f, 2, 0)));
    
    Physics::Component::RigidBody rbB = Physics::Component::RigidBody::CreateDynamic(10.0f);
    boxB.AddComponent<Physics::Component::RigidBody>(core, rbB);
    
    Physics::Component::BoxCollider colliderB(glm::vec3(0.5f));
    boxB.AddComponent<Physics::Component::BoxCollider>(core, colliderB);
    
    // Weld boxes together (Fixed constraint)
    auto constraint = Physics::Component::FixedConstraint::Create(
        boxA, boxB,
        glm::vec3(0.5f, 0, 0),  // Right side of boxA
        glm::vec3(-0.5f, 0, 0)  // Left side of boxB
    );
    
    boxA.AddComponent<Physics::Component::FixedConstraint>(core, constraint);
    
    Logger::Info("Boxes welded together - they move as one object");
}
```

### Example 2: Rope/Chain (Distance Constraint)

```cpp
// Create chain of connected objects
void CreateChain(Engine::Core &core, glm::vec3 startPos, int linkCount) {
    Engine::Entity previousLink;
    
    for (int i = 0; i < linkCount; ++i) {
        // Create link
        auto link = core.CreateEntity();
        
        glm::vec3 position = startPos + glm::vec3(0, -i * 0.8f, 0);
        link.AddComponent<Object::Component::Transform>(core,
            Object::Component::Transform::Create(position));
        
        // Physics
        auto motionType = (i == 0) ? Physics::Component::MotionType::Static
                                    : Physics::Component::MotionType::Dynamic;
        Physics::Component::RigidBody rb;
        if (motionType == Physics::Component::MotionType::Static) {
            rb = Physics::Component::RigidBody::CreateStatic();
        } else {
            rb = Physics::Component::RigidBody::CreateDynamic(1.0f);
        }
        link.AddComponent<Physics::Component::RigidBody>(core, rb);
        
        // Capsule collider (chain link)
        Physics::Component::CapsuleCollider collider(0.4f, 0.1f);
        link.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
        
        // Connect to previous link
        if (i > 0) {
            auto constraint = Physics::Component::DistanceConstraint::Create(
                previousLink,
                link,
                0.8f,  // Distance between links
                glm::vec3(0, -0.2f, 0),  // Bottom of previous link
                glm::vec3(0, 0.2f, 0),   // Top of current link
                Physics::Component::ConstraintSettings::Soft(0.9f, 0.1f)
            );
            
            previousLink.AddComponent<Physics::Component::DistanceConstraint>(core, constraint);
        }
        
        previousLink = link;
    }
    
    Logger::Info("Created chain with {} links", linkCount);
}
```

### Example 3: Pendulum (Point to World)

```cpp
// Create swinging pendulum
void CreatePendulum(Engine::Core &core, glm::vec3 anchorPos) {
    // Pendulum weight (dynamic sphere)
    auto weight = core.CreateEntity();
    
    glm::vec3 weightPos = anchorPos + glm::vec3(0, -3, 0);
    weight.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(weightPos));
    
    Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(5.0f);
    weight.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    Physics::Component::SphereCollider collider(0.3f);
    weight.AddComponent<Physics::Component::SphereCollider>(core, collider);
    
    // Point constraint to world (at anchor)
    auto constraint = Physics::Component::PointConstraint::CreateToWorld(
        weight,
        anchorPos,  // Fixed point in world space
        Physics::Component::ConstraintSettings::Rigid()
    );
    
    weight.AddComponent<Physics::Component::PointConstraint>(core, constraint);
    
    // Give initial push
    Physics::Resource::ForceApplicator::AddImpulse(core, weight, glm::vec3(5, 0, 0));
    
    Logger::Info("Pendulum created at: {}, {}, {}", 
                 anchorPos.x, anchorPos.y, anchorPos.z);
}
```

### Example 4: Ragdoll Arm (Point Constraints)

```cpp
// Create simple ragdoll arm (shoulder + elbow)
void CreateRagdollArm(Engine::Core &core, glm::vec3 shoulderPos) {
    // Upper arm (shoulder to elbow)
    auto upperArm = core.CreateEntity();
    upperArm.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(shoulderPos + glm::vec3(0.5f, 0, 0)));
    
    Physics::Component::RigidBody rbUpper = Physics::Component::RigidBody::CreateDynamic(2.0f);
    upperArm.AddComponent<Physics::Component::RigidBody>(core, rbUpper);
    
    Physics::Component::CapsuleCollider colliderUpper(0.8f, 0.1f);
    upperArm.AddComponent<Physics::Component::CapsuleCollider>(core, colliderUpper);
    
    // Lower arm (elbow to hand)
    auto lowerArm = core.CreateEntity();
    lowerArm.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(shoulderPos + glm::vec3(1.5f, 0, 0)));
    
    Physics::Component::RigidBody rbLower = Physics::Component::RigidBody::CreateDynamic(1.5f);
    lowerArm.AddComponent<Physics::Component::RigidBody>(core, rbLower);
    
    Physics::Component::CapsuleCollider colliderLower(0.8f, 0.08f);
    lowerArm.AddComponent<Physics::Component::CapsuleCollider>(core, colliderLower);
    
    // Shoulder joint (upper arm to world)
    auto shoulderJoint = Physics::Component::PointConstraint::CreateToWorld(
        upperArm,
        shoulderPos,
        Physics::Component::ConstraintSettings::Soft(0.8f, 0.2f)
    );
    upperArm.AddComponent<Physics::Component::PointConstraint>(core, shoulderJoint);
    
    // Elbow joint (upper arm to lower arm)
    auto elbowJoint = Physics::Component::PointConstraint::Create(
        upperArm,
        lowerArm,
        glm::vec3(0.4f, 0, 0),   // End of upper arm
        glm::vec3(-0.4f, 0, 0),  // Start of lower arm
        Physics::Component::ConstraintSettings::Soft(0.9f, 0.1f)
    );
    upperArm.AddComponent<Physics::Component::PointConstraint>(core, elbowJoint);
    
    Logger::Info("Ragdoll arm created");
}
```

### Example 5: Breaking Constraint (Destructible Object)

```cpp
// Create destructible object that breaks under force
void CreateBreakableObject(Engine::Core &core, glm::vec3 position) {
    // Main body (static anchor)
    auto anchor = core.CreateEntity();
    anchor.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position));
    
    Physics::Component::RigidBody rbAnchor = Physics::Component::RigidBody::CreateStatic();
    anchor.AddComponent<Physics::Component::RigidBody>(core, rbAnchor);
    
    Physics::Component::BoxCollider colliderAnchor(glm::vec3(0.5f));
    anchor.AddComponent<Physics::Component::BoxCollider>(core, colliderAnchor);
    
    // Attached piece (dynamic)
    auto piece = core.CreateEntity();
    piece.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(position + glm::vec3(0, -1.5f, 0)));
    
    Physics::Component::RigidBody rbPiece = Physics::Component::RigidBody::CreateDynamic(5.0f);
    piece.AddComponent<Physics::Component::RigidBody>(core, rbPiece);
    
    Physics::Component::BoxCollider colliderPiece(glm::vec3(0.5f));
    piece.AddComponent<Physics::Component::BoxCollider>(core, colliderPiece);
    
    // Breakable fixed constraint
    auto constraint = Physics::Component::FixedConstraint::Create(
        anchor,
        piece,
        glm::vec3(0, -0.5f, 0),  // Bottom of anchor
        glm::vec3(0, 0.5f, 0),   // Top of piece
        Physics::Component::ConstraintSettings::Breakable(100.0f, 50.0f)  // Break force
    );
    
    anchor.AddComponent<Physics::Component::FixedConstraint>(core, constraint);
    
    Logger::Info("Breakable object created (break force: 100N)");
}

// Check if constraint broke
void UpdateBreakableObjects(Engine::Core &core) {
    auto view = core.GetRegistry().view<Physics::Component::FixedConstraint>();
    
    for (auto entity : view) {
        auto &constraint = view.get<Physics::Component::FixedConstraint>(entity);
        
        if (constraint.broken) {
            Logger::Info("Constraint broken!");
            
            // Spawn break effect
            auto &transform = Engine::Entity(entity).GetComponent<Object::Component::Transform>(core);
            Particles::SpawnExplosion(core, transform.position);
            Audio::PlaySound("sounds/break.wav");
            
            // Remove constraint component
            core.GetRegistry().remove<Physics::Component::FixedConstraint>(entity);
        }
    }
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid body entities**
   ```cpp
   if (!bodyA.IsValid(core) || !bodyB.IsValid(core)) {
       Logger::Error("Constraint: Invalid body entity");
       return;
   }
   ```

2. **Self-constraint**
   ```cpp
   if (bodyA == bodyB) {
       Logger::Error("Constraint: Cannot constrain body to itself");
       return;
   }
   ```

3. **Both bodies static**
   ```cpp
   if (rbA.motionType == MotionType::Static && rbB.motionType == MotionType::Static) {
       Logger::Error("Constraint: Both bodies are static (no effect)");
       return;
   }
   ```

4. **Invalid distance**
   ```cpp
   if (maxDistance < minDistance) {
       Logger::Error("DistanceConstraint: maxDistance < minDistance");
       return;
   }
   ```

## 💡 Design Considerations

### Constraint Types Summary

| Type | DOF | Use Case | Performance |
|------|-----|----------|-------------|
| **Fixed** | 0 | Weld, rigid attachment | Fast |
| **Distance** | 5 | Rope, spring, tether | Fast |
| **Point** | 3 | Ragdoll, ball joint | Fast |

### Breaking Constraints

```cpp
// Check constraint forces every frame
if (constraintForce > breakForce || constraintTorque > breakTorque) {
    constraint.broken = true;
    RemoveConstraint(core, entity);
}
```

### Soft vs Rigid Constraints

```cpp
// Rigid (instant response, no spring)
auto settings = ConstraintSettings::Rigid();

// Soft (spring-like, gradual)
auto settings = ConstraintSettings::Soft(0.5f, 0.1f);
```

## 📊 Success Criteria

- [ ] Constraint base implemented
- [ ] FixedConstraint implemented
- [ ] DistanceConstraint implemented
- [ ] PointConstraint implemented
- [ ] ConstraintFactory implemented
- [ ] ConstraintSystem implemented
- [ ] Jolt integration working
- [ ] Constraint breaking works
- [ ] World-constrained bodies work
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 100 constraints)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Constraints: https://jrouwe.github.io/JoltPhysics/
- Unity Joints: https://docs.unity3d.com/Manual/Joints.html
- Godot Joints: https://docs.godotengine.org/en/stable/classes/class_joint3d.html
- Physics Constraints Theory: https://www.toptal.com/game/video-game-physics-part-ii-collision-detection-for-solid-objects

## 🚀 Follow-up Issues

- #016: Hinge & Slider Constraints (with motors)
- #017: 6DOF Constraint (configurable limits)
- #020: Debug Visualization (draw constraints)
- #024: Constraint Solver Tuning (performance optimization)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.6`, `priority:medium`, `constraints`  
**Milestone:** v0.6 - Collision Events & Constraints
