# RigidBody Component Usage Example

## Overview

The `Physics::Component::RigidBody` component provides a clean, user-friendly API for adding physics simulation to entities in EngineSquared. It fully encapsulates Jolt Physics complexity while exposing intuitive properties for developers.

## Basic Example: Falling Cube

```cpp
#include "core/Core.hpp"
#include "Physics.hpp"
#include "Object.hpp"

void CreateFallingCube(Engine::Core& core) {
    // Create entity
    auto cube = core.CreateEntity();

    // Add Transform component (position at 5 units high)
    auto transform = Object::Component::Transform();
    transform.SetPosition(0.0f, 5.0f, 0.0f);
    cube.AddComponent<Object::Component::Transform>(core, transform);

    // Add mesh component (for rendering)
    // ... (mesh creation code)

    // Add physics - simple dynamic body
    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(10.0f); // 10kg mass
    rigidBody.friction = 0.5f;
    rigidBody.restitution = 0.3f; // 30% bounce
    cube.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    // DefaultCollider is created automatically!
    // Cube will now fall and collide with other objects
}
```

## Example: Floor with Custom Collider

```cpp
void CreateFloor(Engine::Core& core) {
    auto floor = core.CreateEntity();

    // Position floor at ground level
    auto transform = Object::Component::Transform();
    transform.SetPosition(0.0f, 0.0f, 0.0f);
    floor.AddComponent<Object::Component::Transform>(core, transform);

    // Add large box collider for floor (10x1x10 meters)
    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(5.0f, 0.5f, 5.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(core, boxCollider);

    // Static rigid body (won't move)
    auto rigidBody = Physics::Component::RigidBody::CreateStatic();
    floor.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
}
```

## Example: Bouncy Ball

```cpp
void CreateBouncyBall(Engine::Core& core) {
    auto ball = core.CreateEntity();

    // Position above floor
    auto transform = Object::Component::Transform();
    transform.SetPosition(0.0f, 10.0f, 0.0f);
    ball.AddComponent<Object::Component::Transform>(core, transform);

    // Sphere collider (approximated with box for now)
    auto collider = Physics::Component::BoxCollider::CreateCube(0.5f); // 1m diameter
    ball.AddComponent<Physics::Component::BoxCollider>(core, collider);

    // Very bouncy physics properties
    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rigidBody.restitution = 0.9f; // 90% bounce
    rigidBody.friction = 0.1f;    // Low friction
    ball.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
}
```

## Example: Kinematic Platform

```cpp
void CreateMovingPlatform(Engine::Core& core) {
    auto platform = core.CreateEntity();

    auto transform = Object::Component::Transform();
    transform.SetPosition(0.0f, 2.0f, 0.0f);
    platform.AddComponent<Object::Component::Transform>(core, transform);

    // Platform collider
    auto collider = Physics::Component::BoxCollider(glm::vec3(2.0f, 0.2f, 2.0f));
    platform.AddComponent<Physics::Component::BoxCollider>(core, collider);

    // Kinematic body - can be moved programmatically but not affected by forces
    auto rigidBody = Physics::Component::RigidBody::CreateKinematic();
    platform.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    // You can now update the Transform in your game loop to move the platform
}
```

## Component Properties

### MotionType
- `Static`: Never moves (walls, floors, static obstacles)
- `Kinematic`: Moves programmatically, not affected by forces
- `Dynamic`: Fully simulated, affected by gravity and forces

### Physical Properties
- `mass`: Body mass in kilograms (dynamic bodies only)
- `friction`: Surface friction coefficient [0, 1]
- `restitution`: Bounciness [0, 1] (0 = no bounce, 1 = perfect bounce)
- `linearDamping`: Air resistance for linear motion
- `angularDamping`: Air resistance for rotation
- `gravityFactor`: Gravity multiplier (1.0 = normal, 0.0 = no gravity)

### Collision Settings
- `objectLayer`: Collision layer for filtering (default: 0 = MOVING)
- `allowSleeping`: Allow body to sleep when inactive (optimization)
- `activation`: Activate immediately or start sleeping

## Automatic Features

### 1. DefaultCollider Auto-Creation
If you add a RigidBody without any collider component, the system automatically creates a `DefaultCollider` based on the entity's mesh bounds:

```cpp
auto entity = core.CreateEntity();
entity.AddComponent<Object::Component::Transform>(core);
entity.AddComponent<Physics::Component::RigidBody>(core, RigidBody::CreateDynamic());
// DefaultCollider created automatically! ✓
```

### 2. Transform Auto-Creation
If you add a RigidBody without a Transform, the system creates a default Transform:

```cpp
auto entity = core.CreateEntity();
entity.AddComponent<Physics::Component::RigidBody>(core, RigidBody::CreateDynamic());
// Transform created automatically! ✓
```

### 3. Internal Component Management
The `RigidBodyInternal` component (which stores Jolt BodyID) is automatically managed:
- Created when RigidBody is added
- Destroyed when RigidBody is removed
- **Never interact with this component directly!**

## Complete Example: Physics Scene

```cpp
#include "core/Core.hpp"
#include "Physics.hpp"
#include "Object.hpp"

void SetupPhysicsScene(Engine::Core& core) {
    // 1. Create static floor
    auto floor = core.CreateEntity();
    floor.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform(glm::vec3(0, 0, 0)));
    floor.AddComponent<Physics::Component::BoxCollider>(core,
        Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f)));
    floor.AddComponent<Physics::Component::RigidBody>(core,
        Physics::Component::RigidBody::CreateStatic());

    // 2. Create falling cube
    auto cube = core.CreateEntity();
    auto cubeTransform = Object::Component::Transform();
    cubeTransform.SetPosition(0.0f, 5.0f, 0.0f);
    cube.AddComponent<Object::Component::Transform>(core, cubeTransform);

    auto cubeRb = Physics::Component::RigidBody::CreateDynamic(10.0f);
    cubeRb.friction = 0.5f;
    cubeRb.restitution = 0.3f;
    cube.AddComponent<Physics::Component::RigidBody>(core, cubeRb);
    // DefaultCollider auto-created!

    // 3. Run game loop
    while (running) {
        core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().Update(core);
        // Physics updates automatically!
        // Cube falls, hits floor, bounces slightly
    }
}
```

## Factory Methods

Convenient factory methods for common body types:

```cpp
// Static body (mass = 0, doesn't activate)
auto staticBody = RigidBody::CreateStatic();

// Kinematic body (mass = 0, activates)
auto kinematicBody = RigidBody::CreateKinematic();

// Dynamic body with custom mass
auto dynamicBody = RigidBody::CreateDynamic(5.0f); // 5kg
```

## Collider Types

### BoxCollider (Explicit)
```cpp
// Custom box dimensions
auto box = BoxCollider(glm::vec3(1.0f, 2.0f, 1.0f)); // half-extents

// Cube shorthand
auto cube = BoxCollider::CreateCube(1.0f); // 2x2x2 cube
```

### DefaultCollider (Auto-created)
```cpp
// Created automatically if no collider exists
// Uses mesh bounds to determine size
// Can be customized after creation
```

## System Initialization

The RigidBody system is automatically initialized when the Physics plugin is bound:

```cpp
Physics::Plugin physicsPlugin;
physicsPlugin.RegisterSystems(core);
physicsPlugin.Bind();

// RigidBodySystem hooks are now registered!
// No additional setup needed
```

## Notes

- **Thread Safety**: RigidBody operations must occur on the main thread
- **Performance**: Jolt Physics handles sleeping bodies efficiently
- **Collision Layers**: Use `objectLayer` to control what collides with what
- **Debugging**: Enable logging with `Logger::SetLevel(LogLevel::Debug)`

## Future Enhancements

- [ ] MeshCollider for complex shapes
- [ ] SphereCollider for spherical objects
- [ ] CapsuleCollider for character controllers
- [ ] Compound colliders (multiple shapes per body)
- [ ] Constraints and joints
- [x] **Force/impulse application API** (✅ Completed in v0.3)
- [ ] Collision callback system

---

## Applying Forces and Impulses (v0.3 - Issue #001)

The Force Applicator API provides 6 functions to apply forces and impulses to dynamic rigid bodies.

### Overview

| Function | Type | Effect | Use Case |
|----------|------|--------|----------|
| `AddForce()` | Continuous | Linear acceleration | Thrusters, wind, magnetism |
| `AddForceAtPoint()` | Continuous | Linear + rotation | Off-center push, realistic impacts |
| `AddTorque()` | Continuous | Angular acceleration | Motors, gyroscopes |
| `AddImpulse()` | Instant | Velocity change | Explosions, jumps, collisions |
| `AddImpulseAtPoint()` | Instant | Velocity + spin | Baseball bat, projectile impacts |
| `AddAngularImpulse()` | Instant | Angular velocity | Instant spin-up/down |

### Key Differences

**Forces vs Impulses:**
- **Force**: Applied continuously (call every frame in game loop). Accelerates body over time according to F = ma.
- **Impulse**: Applied once for instant effect. Changes velocity immediately (p = mv).

**Linear vs Angular:**
- **Linear**: Affects translation (movement through space)
- **Angular**: Affects rotation (spinning)

**Center vs At Point:**
- **Center**: Simple push at center of mass
- **At Point**: Creates both translation AND rotation (torque)

### Example 1: Continuous Upward Force (like a thruster)

```cpp
#include "Physics.hpp"

void UpdateThruster(Engine::Core& core, Engine::Entity rocket) {
    // Call this every frame in your game loop
    glm::vec3 thrustForce(0.0f, 100.0f, 0.0f);  // 100N upward
    Physics::Resource::AddForce(core, rocket, thrustForce);
}
```

### Example 2: Explosion with Radial Impulses

```cpp
void CreateExplosion(Engine::Core& core, glm::vec3 center, float radius, float strength) {
    auto& registry = core.GetRegistry();
    
    // Find all dynamic bodies within radius
    registry.view<Physics::Component::RigidBody, Object::Component::Transform>().each(
        [&](Engine::Entity entity, const auto& rb, const auto& transform) {
            if (rb.motionType != Physics::Component::MotionType::Dynamic)
                return;
            
            float distance = glm::distance(transform.position, center);
            if (distance > radius)
                return;
            
            // Calculate impulse direction and magnitude
            glm::vec3 direction = glm::normalize(transform.position - center);
            float falloff = 1.0f - (distance / radius);  // Closer = stronger
            glm::vec3 impulse = direction * strength * falloff;
            
            // Apply instant velocity change
            Physics::Resource::AddImpulse(core, entity, impulse);
        });
}

// Usage:
CreateExplosion(core, glm::vec3(0, 5, 0), 10.0f, 50.0f);  // 10m radius, 50 N·s
```

### Example 3: Spinning Object with Torque

```cpp
void CreateSpinner(Engine::Core& core) {
    auto spinner = Engine::Entity::Create(core);
    
    auto& rb = spinner.AddComponent<Physics::Component::RigidBody>(core);
    rb.motionType = Physics::Component::MotionType::Dynamic;
    rb.mass = 5.0f;
    rb.angularDamping = 0.05f;  // Low damping for sustained spin
    
    // Apply torque every frame
    auto spinSystem = [spinner](Engine::Core& core) {
        glm::vec3 torque(0.0f, 10.0f, 0.0f);  // 10 N·m around Y-axis
        Physics::Resource::AddTorque(core, spinner, torque);
    };
    
    core.RegisterSystem<Engine::Scheduler::Update>(spinSystem);
}
```

### Example 4: Opening a Door (Force at Point)

```cpp
void PushDoor(Engine::Core& core, Engine::Entity door, glm::vec3 pushPoint) {
    // Push at the edge of the door (far from hinge)
    // This creates both translation and rotation
    glm::vec3 pushForce(0.0f, 0.0f, 15.0f);  // 15N forward
    
    Physics::Resource::AddForceAtPoint(core, door, pushForce, pushPoint);
}

// Usage:
glm::vec3 doorEdge(1.0f, 1.5f, 0.0f);  // Right edge at handle height
PushDoor(core, doorEntity, doorEdge);
```

### Example 5: Baseball Bat Hit (Impulse at Point)

```cpp
void HitBall(Engine::Core& core, Engine::Entity ball, glm::vec3 batContactPoint) {
    // Hit slightly above center to create backspin
    glm::vec3 batImpulse(20.0f, 5.0f, 0.0f);  // Forward and up
    
    // Creates both velocity and spin
    Physics::Resource::AddImpulseAtPoint(core, ball, batImpulse, batContactPoint);
}

// Usage:
glm::vec3 ballPosition(0, 1, 0);
glm::vec3 hitPoint = ballPosition + glm::vec3(0, 0.05f, 0);  // Slightly above center
HitBall(core, baseball, hitPoint);
```

### Example 6: Instant Spin Change (Angular Impulse)

```cpp
void SpinUpFlywheel(Engine::Core& core, Engine::Entity flywheel) {
    // Instant angular velocity change (like a motor startup)
    glm::vec3 angularImpulse(0.0f, 20.0f, 0.0f);  // 20 N·m·s around Y
    
    Physics::Resource::AddAngularImpulse(core, flywheel, angularImpulse);
}
```

### Error Handling

All functions handle common error cases gracefully:

```cpp
// Invalid entity - logs error, doesn't crash
Engine::Entity invalid = Engine::Entity(entt::entity(999999));
Physics::Resource::AddForce(core, invalid, glm::vec3(1, 0, 0));
// Output: [error] AddForce: Entity 999999 has no RigidBody component

// Non-dynamic body - logs warning, ignores force
Engine::Entity staticWall = /*...*/;
Physics::Resource::AddForce(core, staticWall, glm::vec3(1, 0, 0));
// Output: [warning] AddForce: Cannot apply force/impulse to non-Dynamic body

// NaN or Inf vectors - rejected with error
glm::vec3 invalid(std::numeric_limits<float>::quiet_NaN(), 0, 0);
Physics::Resource::AddForce(core, entity, invalid);
// Output: [error] AddForce: Invalid force vector (NaN or Inf): vec3(nan, 0, 0)
```

### Best Practices

**When to use Forces:**
- Continuous effects (thrusters, wind, gravity modifiers)
- Gradual acceleration
- Apply every frame in game loop

**When to use Impulses:**
- One-time events (explosions, jumps, collisions)
- Instant velocity changes
- Apply once when event occurs

**Performance Tips:**
- Forces are cheaper than impulses for continuous effects
- Use `AddForceAtPoint` and `AddImpulseAtPoint` only when rotation is needed
- Validate vectors before applying (avoid NaN/Inf)

**Physics Units:**
- Force: Newtons (N) = kg·m/s²
- Impulse: Newton-seconds (N·s) = kg·m/s
- Torque: Newton-meters (N·m) = kg·m²/s²
- Angular Impulse: Newton-meter-seconds (N·m·s) = kg·m²/s

### Complete API Reference

```cpp
namespace Physics::Resource {

// Continuous forces (apply each frame)
void AddForce(Engine::Core& core, Engine::Entity entity, const glm::vec3& force);
void AddForceAtPoint(Engine::Core& core, Engine::Entity entity, 
                     const glm::vec3& force, const glm::vec3& worldPoint);
void AddTorque(Engine::Core& core, Engine::Entity entity, const glm::vec3& torque);

// Impulses (apply once)
void AddImpulse(Engine::Core& core, Engine::Entity entity, const glm::vec3& impulse);
void AddImpulseAtPoint(Engine::Core& core, Engine::Entity entity,
                       const glm::vec3& impulse, const glm::vec3& worldPoint);
void AddAngularImpulse(Engine::Core& core, Engine::Entity entity, 
                       const glm::vec3& angularImpulse);

} // namespace Physics::Resource
```

---

## Velocity Control API (v0.3 - Issue #002)

The Velocity Control API provides direct control over linear and angular velocities, bypassing force/impulse simulation for immediate response.

### Overview

| Function | Get/Set | Type | Use Case |
|----------|---------|------|----------|
| `SetLinearVelocity()` | Set | Linear | Direct movement control (character, platform) |
| `GetLinearVelocity()` | Get | Linear | Read current velocity for decisions |
| `AddLinearVelocity()` | Modify | Linear | Add velocity delta (jumps, boosts) |
| `SetAngularVelocity()` | Set | Angular | Direct rotation control (spinning objects) |
| `GetAngularVelocity()` | Get | Angular | Read current rotation speed |
| `AddAngularVelocity()` | Modify | Angular | Add rotation delta (spin effects) |

### Key Concepts

**Velocity vs Forces:**
- **Velocity Control**: Direct, immediate, frame-rate independent. Perfect for player control.
- **Forces**: Realistic physics simulation with acceleration. Better for NPCs and physics objects.

**When to use Velocity:**
- Character controllers (platformers, FPS)
- Precise movement requirements
- Kinematic-like behavior on dynamic bodies
- Instant response needed

**When to use Forces:**
- Realistic vehicle physics
- Natural-feeling interactions
- Physics-driven gameplay
- Gradual acceleration desired

### Example 1: Character Movement (Velocity-based)

```cpp
void UpdateCharacterMovement(Engine::Core& core, Engine::Entity character,
                             const glm::vec2& input, float speed) {
    // Get current velocity to preserve Y (gravity)
    glm::vec3 currentVel = Physics::Resource::GetLinearVelocity(core, character);

    // Calculate horizontal movement
    glm::vec3 targetVel(input.x * speed, currentVel.y, input.y * speed);

    // Apply velocity
    Physics::Resource::SetLinearVelocity(core, character, targetVel);
}

void CharacterJump(Engine::Core& core, Engine::Entity character, float jumpForce) {
    // Add upward velocity (preserves horizontal movement)
    Physics::Resource::AddLinearVelocity(core, character, glm::vec3(0.0f, jumpForce, 0.0f));
}
```

### Example 2: Spinning Platform (Constant Rotation)

```cpp
void CreateRotatingPlatform(Engine::Core& core, Engine::Entity platform, float rpm) {
    // Convert RPM to rad/s
    float radiansPerSecond = (rpm * 2.0f * glm::pi<float>()) / 60.0f;

    // Set constant angular velocity
    Physics::Resource::SetAngularVelocity(core, platform, 
                                         glm::vec3(0.0f, radiansPerSecond, 0.0f));
    
    // Platform spins continuously without torque!
}
```

### Example 3: Speed Limiter

```cpp
void LimitSpeed(Engine::Core& core, Engine::Entity entity, float maxSpeed) {
    glm::vec3 velocity = Physics::Resource::GetLinearVelocity(core, entity);
    float speed = glm::length(velocity);

    if (speed > maxSpeed) {
        // Clamp to max speed
        velocity = glm::normalize(velocity) * maxSpeed;
        Physics::Resource::SetLinearVelocity(core, entity, velocity);
    }
}
```

### Example 4: Dash Mechanic

```cpp
void PlayerDash(Engine::Core& core, Engine::Entity player, glm::vec3 direction, float dashSpeed) {
    // Add instant velocity in dash direction
    glm::vec3 dashVelocity = glm::normalize(direction) * dashSpeed;
    Physics::Resource::AddLinearVelocity(core, player, dashVelocity);
}
```

### Example 5: Oscillating Object

```cpp
void UpdateOscillator(Engine::Core& core, Engine::Entity entity, float time) {
    // Get current position
    auto& transform = core.GetRegistry().get<Object::Component::Transform>(entity.Get());
    glm::vec3 pos = transform.GetPosition();

    // Reverse velocity at bounds
    glm::vec3 velocity = Physics::Resource::GetLinearVelocity(core, entity);
    
    if (pos.y > 5.0f && velocity.y > 0.0f) {
        // Hit top, reverse
        Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(0.0f, -3.0f, 0.0f));
    } else if (pos.y < 0.0f && velocity.y < 0.0f) {
        // Hit bottom, reverse
        Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(0.0f, 3.0f, 0.0f));
    }
}
```

### Best Practices

**When to use Velocity Control:**
- Player character controllers
- Platformer movement
- Precise, predictable movement
- Frame-rate independent behavior
- Kinematic-style control on dynamic bodies

**When to use Forces instead:**
- Realistic vehicle physics
- NPCs with natural movement
- Physics-driven interactions
- Gradual acceleration effects

**Performance Tips:**
- Get/Set operations are O(1) - very fast
- No accumulation needed (unlike forces)
- Safe to call multiple times per frame
- Automatically activates sleeping bodies

**Physics Units:**
- Linear Velocity: meters per second (m/s)
- Angular Velocity: radians per second (rad/s)
- Use `glm::radians()` to convert degrees → radians
- Use `glm::degrees()` to convert radians → degrees

**Common Patterns:**

```cpp
// Stop object instantly
Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(0.0f));

// Check if moving
glm::vec3 vel = Physics::Resource::GetLinearVelocity(core, entity);
bool isMoving = glm::length(vel) > 0.1f;

// Preserve Y velocity (gravity) when changing X/Z
glm::vec3 currentVel = Physics::Resource::GetLinearVelocity(core, entity);
Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(newX, currentVel.y, newZ));

// Gradual acceleration (call each frame)
Physics::Resource::AddLinearVelocity(core, entity, glm::vec3(0.1f, 0.0f, 0.0f));
```

### Complete API Reference

```cpp
namespace Physics::Resource {

// Linear velocity control
void SetLinearVelocity(Engine::Core& core, Engine::Entity entity, 
                       const glm::vec3& velocity);
glm::vec3 GetLinearVelocity(Engine::Core& core, Engine::Entity entity);
void AddLinearVelocity(Engine::Core& core, Engine::Entity entity, 
                       const glm::vec3& deltaVelocity);

// Angular velocity control
void SetAngularVelocity(Engine::Core& core, Engine::Entity entity, 
                        const glm::vec3& angularVelocity);
glm::vec3 GetAngularVelocity(Engine::Core& core, Engine::Entity entity);
void AddAngularVelocity(Engine::Core& core, Engine::Entity entity, 
                        const glm::vec3& deltaAngularVelocity);

} // namespace Physics::Resource
```

---

### Comparison: Velocity vs Forces

| Aspect | Velocity Control (#002) | Forces/Impulses (#001) |
|--------|------------------------|------------------------|
| **Response** | Instant | Gradual (forces) or Instant (impulses) |
| **Frame-rate** | Independent | Dependent (forces need integration) |
| **Realism** | Less realistic | More realistic |
| **Use case** | Player control | NPC behavior, vehicles |
| **Complexity** | Simple | More complex |
| **Physics interaction** | Bypasses simulation | Part of simulation |

**Example comparison:**

```cpp
// Character jump with IMPULSE (realistic):
Physics::Resource::AddImpulse(core, character, glm::vec3(0.0f, 50.0f, 0.0f));
// → Feels "heavy" and physical

// Character jump with VELOCITY (instant):
Physics::Resource::AddLinearVelocity(core, character, glm::vec3(0.0f, 5.0f, 0.0f));
// → Feels "snappy" and responsive (better for platformers)
```

### See Also

- [VelocityControlExample.cpp](../examples/physics_usage/src/examples/VelocityControlExample.cpp) - Complete working examples
- [Issue #002](./issues/002-velocity-control-api.md) - Original specification
- [VelocityController.hpp](../src/plugin/physics/src/resource/VelocityController.hpp) - Implementation details
- [forces_demo.cpp](../examples/physics_usage/forces_demo.cpp) - Compare with Force Applicator API
- [Issue #001](./issues/001-forces-impulses-api.md) - Force Applicator specification
- [ForceApplicator.hpp](../src/plugin/physics/src/resource/ForceApplicator.hpp) - Implementation details

````
