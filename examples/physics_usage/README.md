# Physics Usage Examples

This directory contains **modular plugin-based examples** demonstrating the **Physics plugin** in EngineSquared.

## Architecture Overview

Each example is implemented as a **separate plugin** that can be run independently or combined:
- **`src/examples/FallingObjectsExample.hpp/cpp`** - Original physics demo
- **`src/examples/ForcesImpulsesExample.hpp/cpp`** - Issue #001 API demo
- **`src/main.cpp`** - Interactive menu to select examples

This architecture makes it easy to add new examples for future issues - just create a new plugin file!

## Available Examples

### 1. Falling Objects Example (Original)
Demonstrates RigidBody components, colliders, and different motion types in a full simulation loop.

**Features:**
- Static bodies (immovable floor)
- Dynamic bodies (falling cubes with mass, friction, restitution)
- Kinematic bodies (programmable platform)
- Colliders (BoxCollider and DefaultCollider)
- Physics properties (mass, friction, bounciness, damping)
- Simulation loop with fixed timestep (100 steps)

**What you'll see:**
- A static floor
- 3 falling cubes with different masses
- A bouncy ball
- Statistics printed every 10 steps

### 2. Forces & Impulses Example (v0.3 - Issue #001)
Demonstrates the Force Applicator API for applying forces and impulses to rigid bodies.

**Features:**
- **AddForce()** - Continuous force at center (thrusters, wind)
- **AddForceAtPoint()** - Continuous force at position (creates rotation)
- **AddTorque()** - Continuous rotational force (motors, spinning)
- **AddImpulse()** - Instant velocity change (explosions, jumps)
- **AddImpulseAtPoint()** - Instant velocity + spin (bat hits, impacts)
- **AddAngularImpulse()** - Instant angular velocity change (spin-up)

**What you'll see:**
- Propelled cube rising with continuous force
- Explosion with radial impulses on debris
- Spinner rotating with continuous torque
- Door opening with force at point
- Flywheel spinning with angular impulse
- Baseball hit with impulse at point

## Building and Running

### From the example directory:

```bash
cd examples/physics_usage
xmake -P .
xmake run PhysicsUsage -P .
```

### From the repository root:

```bash
xmake build PhysicsUsage
xmake run PhysicsUsage
```

## Usage

### Interactive Menu

Run without arguments to get an interactive menu:

```bash
./PhysicsUsage
```

Output:
```
========================================
  EngineSquared - Physics Examples
========================================

Available Examples:
  1. Falling Objects Example (Original)
  2. Forces & Impulses Example (Issue #001)
  3. Run All Examples
  0. Exit

Select example (0-3): 
```

### Command-Line Arguments

Run specific examples directly:

```bash
./PhysicsUsage 1         # Falling Objects
./PhysicsUsage falling   # Same as above

./PhysicsUsage 2         # Forces & Impulses
./PhysicsUsage forces    # Same as above

./PhysicsUsage 3         # All examples
./PhysicsUsage all       # Same as above

./PhysicsUsage --help    # Show help
```

## Example Output

### Falling Objects Example

```
→ Running Falling Objects Example...

[Falling Objects Example]
Creating physics scene...
  - Floor created (static body)
  - Cube created at (0, 10, 0) with mass 5kg
  - Cube created at (2, 15, 2) with mass 10kg
  - Cube created at (-2, 12, -2) with mass 2kg
  - Bouncy ball created at (3, 20, 0)
  - Kinematic platform created
✓ Scene setup complete!

[Step 10] Dynamic: 4 | Static: 1 | Kinematic: 1
  Dynamic body positions:
    Entity 1: (0, 9.785, 0)
    Entity 2: (2, 14.785, 2)
    Entity 3: (-2, 11.785, -2)
    Entity 4: (3, 19.785, 0)

[Step 100] Dynamic: 4 | Static: 1 | Kinematic: 1
  Dynamic body positions:
    Entity 1: (0, -4.2, 0)
    ...

✓ Simulation Complete! Ran 100 steps
```

### Forces & Impulses Example

```
→ Running Forces & Impulses Example...

=================================================
  Forces & Impulses Example (Issue #001)
=================================================

[Example 1] Propelled Cube - Continuous Upward Force
  ✓ Created dynamic cube (mass = 2 kg)
  → Will apply 20N upward force each frame

[Example 2] Explosion - Radial Impulses
  ✓ Created 4 debris cubes
  → Will apply explosive impulses from center

[Example 3] Spinner - Continuous Torque
  ✓ Created spinner object (mass = 3 kg)
  → Will apply 10 N·m torque around Y-axis

[Example 4] Door - Force at Point
  ✓ Created door (mass = 5 kg)
  ✓ Applied 15N force at edge (creates rotation)

[Example 5] Flywheel - Angular Impulse
  ✓ Created flywheel (mass = 10 kg)
  ✓ Applied 20 N·m·s angular impulse (instant spin)

[Example 6] Baseball Bat Hit - Impulse at Point
  ✓ Created baseball (mass = 145 g)
  ✓ Applied impulse at point (backspin launch)

✓ All 6 examples created and initialized!

[Explosion] Applying radial impulses...
[Physics Update] Applying continuous forces...

=================================================
  API Summary
=================================================

✓ AddForce()           : Continuous force at center
✓ AddForceAtPoint()    : Continuous force creating rotation
✓ AddTorque()          : Continuous rotational force
✓ AddImpulse()         : Instant velocity change
✓ AddImpulseAtPoint()  : Instant velocity + spin
✓ AddAngularImpulse()  : Instant angular velocity

All 6 API functions demonstrated successfully! ✅
```

## Adding New Examples

To add a new example for a future issue (e.g., #002 - Velocity Control):

1. **Create the plugin files**:
   ```bash
   touch src/examples/VelocityControlExample.hpp
   touch src/examples/VelocityControlExample.cpp
   ```

2. **Implement the plugin** (in `VelocityControlExample.hpp`):
   ```cpp
   #pragma once
   #include "Engine.hpp"
   #include "Physics.hpp"
   
   namespace Examples {
   class VelocityControlExample : public Engine::APlugin {
   public:
       explicit VelocityControlExample(Engine::Core& core) : APlugin(core) {}
       void Bind() override;
   };
   }
   ```

3. **Implement systems** (in `VelocityControlExample.cpp`):
   ```cpp
   #include "VelocityControlExample.hpp"
   
   namespace Examples {
   
   static void SetupSceneSystem(Engine::Core& core) {
       // Your scene setup
   }
   
   static void VelocityDemoSystem(Engine::Core& core) {
       // Your velocity demonstrations
   }
   
   static void CleanupSystem(Engine::Core& core) {
       // Cleanup entities
   }
   
   void VelocityControlExample::Bind() {
       RequirePlugins<Physics::Plugin>();
       RegisterSystems<Engine::Scheduler::Startup>(SetupSceneSystem, CleanupSystem);
       RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(VelocityDemoSystem);
   }
   
   }
   ```

4. **Add to main menu** (in `main.cpp`):
   ```cpp
   #include "examples/VelocityControlExample.hpp"
   
   // Add menu option and case
   case 3:
       core.AddPlugins<Examples::VelocityControlExample>();
       break;
   ```

5. **Compile and run**:
   ```bash
   xmake && ./PhysicsUsage 3
   ```

That's it! The example is now part of the system.

## Key Code Patterns

### Creating a Static Floor

```cpp
auto floor = core.CreateEntity();

auto transform = Object::Component::Transform();
transform.SetPosition(0.0f, 0.0f, 0.0f);
floor.AddComponent<Object::Component::Transform>(core, transform);

auto boxCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
floor.AddComponent<Physics::Component::BoxCollider>(core, boxCollider);

floor.AddComponent<Physics::Component::RigidBody>(
    core, 
    Physics::Component::RigidBody::CreateStatic()
);
```

### Creating a Dynamic Cube

```cpp
auto cube = core.CreateEntity();

auto transform = Object::Component::Transform();
transform.SetPosition(0.0f, 10.0f, 0.0f);
cube.AddComponent<Object::Component::Transform>(core, transform);

auto rigidBody = Physics::Component::RigidBody::CreateDynamic(5.0f); // 5kg
rigidBody.friction = 0.5f;
rigidBody.restitution = 0.3f; // 30% bounce
cube.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

// DefaultCollider is created automatically!
```

### Applying Forces (Issue #001)

```cpp
// Continuous force at center (like a thruster)
Physics::Resource::AddForce(core, entity, glm::vec3(0.0f, 20.0f, 0.0f));

// Instant impulse (like an explosion)
Physics::Resource::AddImpulse(core, entity, glm::vec3(10.0f, 50.0f, 0.0f));

// Torque for rotation (like a motor)
Physics::Resource::AddTorque(core, entity, glm::vec3(0.0f, 10.0f, 0.0f));

// See RIGIDBODY_USAGE.md for complete API documentation
```

## Concepts Demonstrated

### Motion Types

- **Static**: Never moves (floor, walls)
- **Dynamic**: Affected by gravity and forces (cubes, balls)
- **Kinematic**: Moves programmatically, not by forces (platforms, doors)

### Physical Properties

- **mass**: Object weight in kilograms (dynamic bodies only)
- **friction**: Surface friction coefficient [0, 1]
- **restitution**: Bounciness [0, 1] (0 = no bounce, 1 = perfect bounce)
- **linearDamping**: Air resistance for linear motion
- **angularDamping**: Air resistance for rotation

### Colliders

- **BoxCollider**: Explicitly defined box shape
- **DefaultCollider**: Automatically created from mesh bounds

## Dependencies

- `EngineSquaredCore`: Core engine functionality
- `PluginPhysics`: Physics simulation (Jolt Physics)
- `PluginObject`: Transform component
- External: `entt`, `glm`, `joltphysics`, `spdlog`, `fmt`

## Project Structure

```
examples/physics_usage/
├── src/
│   ├── main.cpp                      # Interactive menu system
│   └── examples/
│       ├── FallingObjectsExample.hpp # Original physics demo
│       ├── FallingObjectsExample.cpp
│       ├── ForcesImpulsesExample.hpp # Issue #001 API demo
│       └── ForcesImpulsesExample.cpp
├── xmake.lua                         # Build configuration
└── README.md                         # This file
```

## Next Steps

After understanding these examples, you can:
- Implement Issue #002 (Velocity Control API) as a new plugin
- Add more complex collision shapes (sphere, mesh colliders)
- Handle collision events with callbacks
- Create joints and constraints between bodies
- Integrate with rendering to visualize the simulation

## Troubleshooting

**Physics not updating?**
- Make sure plugins are registered with `core.AddPlugins<Physics::Plugin>()`
- Verify systems are registered in the correct scheduler

**Bodies falling through floor?**
- Check that floor has a collider component
- Verify floor is marked as Static
- Ensure objectLayer settings are correct

**Bodies not colliding?**
- Verify both entities have RigidBody components
- Check that colliders exist (BoxCollider or DefaultCollider)
- Review collision layer configuration in PhysicsManager

**Compilation errors with entt?**
- Empty tag components (no data members) are not passed to lambda parameters
- Use `entt::entity` as first parameter, not `Engine::Entity`
- Convert with `Engine::Entity(entity)` when calling Engine functions

## See Also

- [RIGIDBODY_USAGE.md](../../docs/RIGIDBODY_USAGE.md) - Complete RigidBody component documentation including Force Applicator API
- [PhysicsPluginRoadmap.md](../../docs/PhysicsPluginRoadmap.md) - Future physics features (v0.3 - v0.9)
- [Jolt Physics Documentation](https://github.com/jrouwe/JoltPhysics) - Underlying physics engine
