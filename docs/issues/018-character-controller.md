# Issue #018: Implement Character Controller (Physics-based Movement)

**Milestone:** v0.7 - Advanced Physics  
**Priority:** 🔴 CRITICAL  
**Estimated Effort:** 5-6 days  
**Dependencies:** #001-012 (Core Physics + Queries + Events)  
**Related Issues:** #009 (Raycast), #012 (Collision Filtering), #013 (Collision Events), #019 (Vehicle Physics)  
**Follow-up Issues:** #024 (Debug Visualization), #026 (Editor Integration)

---

## 📋 Description

Implement a physics-based character controller for player/NPC movement. Handles collisions, slopes, stairs, ground detection, and provides smooth, responsive movement with proper physics interaction.

**This is NOT about input handling** - the Input plugin already handles that. This controller focuses on **physics-based movement logic**.

## 🎯 Goals

- ✅ Implement CharacterController component
- ✅ Ground detection (grounded state)
- ✅ Slope handling (walkable angle)
- ✅ Stair stepping (automatic step-up)
- ✅ Move function (input-driven movement)
- ✅ Jump mechanics
- ✅ Collision response (push objects)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   └── CharacterController.hpp     # New file
├── resource/
│   ├── CharacterMovement.hpp       # New file
│   └── CharacterMovement.cpp       # New file
├── system/
│   ├── CharacterControllerSystem.hpp   # New file
│   └── CharacterControllerSystem.cpp   # New file
└── tests/
    └── CharacterControllerTest.cpp     # New file
```

### CharacterController Component

```cpp
/**************************************************************************
 * @file CharacterController.hpp
 * @brief Character controller component (physics-based movement)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Ground state
 */
enum class GroundState {
    Grounded,      ///< On ground (can walk/jump)
    InAir,         ///< In air (falling/jumping)
    Sliding        ///< On steep slope (sliding down)
};

/**
 * @brief Character controller settings
 */
struct CharacterControllerSettings {
    /// Movement
    float walkSpeed = 5.0f;           ///< Walk speed (m/s)
    float runSpeed = 10.0f;           ///< Run speed (m/s)
    float acceleration = 50.0f;       ///< Acceleration (m/s²)
    float airControl = 0.3f;          ///< Air control factor [0, 1]
    
    /// Jump
    float jumpHeight = 2.0f;          ///< Jump height (meters)
    float gravity = -20.0f;           ///< Gravity (m/s²) - negative for down
    bool canDoubleJump = false;       ///< Allow double jump
    
    /// Ground detection
    float groundCheckDistance = 0.1f; ///< Distance for ground raycast
    float slopeLimit = 45.0f;         ///< Max walkable slope (degrees)
    
    /// Stairs
    float maxStepHeight = 0.3f;       ///< Max step-up height (meters)
    bool enableStepUp = true;         ///< Auto step-up stairs
    
    /// Collision
    float pushForce = 5.0f;           ///< Force to push objects (Newtons)
    float radius = 0.5f;              ///< Controller capsule radius
    float height = 2.0f;              ///< Controller capsule height
    
    /**
     * @brief Create default settings
     * @return CharacterControllerSettings
     */
    static CharacterControllerSettings Default() {
        return {};
    }
    
    /**
     * @brief Create fast character settings
     * @return CharacterControllerSettings
     */
    static CharacterControllerSettings Fast() {
        CharacterControllerSettings s;
        s.walkSpeed = 8.0f;
        s.runSpeed = 15.0f;
        s.jumpHeight = 2.5f;
        return s;
    }
    
    /**
     * @brief Create platformer settings (high jump, tight control)
     * @return CharacterControllerSettings
     */
    static CharacterControllerSettings Platformer() {
        CharacterControllerSettings s;
        s.walkSpeed = 6.0f;
        s.runSpeed = 12.0f;
        s.jumpHeight = 3.0f;
        s.gravity = -25.0f;
        s.airControl = 0.8f;
        s.canDoubleJump = true;
        s.maxStepHeight = 0.5f;
        return s;
    }
};

/**
 * @brief Character controller component
 * 
 * Physics-based character movement controller. Handles collisions,
 * ground detection, slopes, stairs, and provides responsive movement.
 * 
 * **Features:**
 * - Ground detection (grounded/in-air/sliding)
 * - Slope handling (auto-climb walkable slopes)
 * - Stair stepping (auto step-up)
 * - Jump mechanics (single/double jump)
 * - Air control (limited movement in air)
 * - Push objects (apply force on collision)
 * 
 * **Usage with Input plugin:**
 * The Input plugin handles keyboard/mouse input. This controller
 * provides the Move() function to apply movement based on input.
 * 
 * @example
 * @code
 * // Setup character controller
 * auto player = core.CreateEntity();
 * player.AddComponent<Object::Component::Transform>(core,
 *     Object::Component::Transform::Create(glm::vec3(0, 5, 0)));
 * 
 * auto rb = Physics::Component::RigidBody::CreateDynamic(80.0f);
 * rb.lockRotationX = true;  // Prevent tipping over
 * rb.lockRotationZ = true;
 * player.AddComponent<Physics::Component::RigidBody>(core, rb);
 * 
 * auto controller = Physics::Component::CharacterController::Create(
 *     Physics::Component::CharacterControllerSettings::Default()
 * );
 * player.AddComponent<Physics::Component::CharacterController>(core, controller);
 * 
 * // Capsule collider (character shape)
 * Physics::Component::CapsuleCollider collider(1.0f, 0.5f);
 * player.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
 * 
 * // In update loop (with Input plugin):
 * glm::vec3 input = glm::vec3(0);
 * if (inputManager.IsKeyPressed(GLFW_KEY_W)) input.z += 1;
 * if (inputManager.IsKeyPressed(GLFW_KEY_S)) input.z -= 1;
 * if (inputManager.IsKeyPressed(GLFW_KEY_A)) input.x -= 1;
 * if (inputManager.IsKeyPressed(GLFW_KEY_D)) input.x += 1;
 * 
 * Physics::Resource::CharacterMovement::Move(core, player, input, false);
 * 
 * if (inputManager.IsKeyPressed(GLFW_KEY_SPACE)) {
 *     Physics::Resource::CharacterMovement::Jump(core, player);
 * }
 * @endcode
 */
struct CharacterController {
    /// Settings
    CharacterControllerSettings settings;
    
    /// State (read-only, updated by system)
    GroundState groundState = GroundState::InAir;
    glm::vec3 velocity = glm::vec3(0.0f);     ///< Current velocity
    glm::vec3 groundNormal = glm::vec3(0, 1, 0); ///< Ground normal
    bool isGrounded = false;                   ///< On ground
    bool canJump = true;                       ///< Can jump now
    int jumpCount = 0;                         ///< Jumps performed (for double jump)
    
    /// Internal (Jolt)
    uint32_t joltCharacterId = 0;              ///< Jolt character ID
    
    /**
     * @brief Create character controller
     * @param controllerSettings Settings
     * @return CharacterController
     */
    static CharacterController Create(
        const CharacterControllerSettings &controllerSettings = CharacterControllerSettings::Default()
    ) {
        CharacterController c;
        c.settings = controllerSettings;
        return c;
    }
    
    /**
     * @brief Check if on ground
     * @return true if grounded
     */
    bool IsGrounded() const {
        return groundState == GroundState::Grounded;
    }
    
    /**
     * @brief Check if on steep slope (sliding)
     * @return true if sliding
     */
    bool IsSliding() const {
        return groundState == GroundState::Sliding;
    }
    
    /**
     * @brief Check if in air
     * @return true if in air
     */
    bool IsInAir() const {
        return groundState == GroundState::InAir;
    }
    
    /**
     * @brief Get ground angle (degrees)
     * @return Angle from horizontal
     */
    float GetGroundAngle() const {
        return glm::degrees(glm::acos(glm::dot(groundNormal, glm::vec3(0, 1, 0))));
    }
    
    /**
     * @brief Check if can walk on current ground
     * @return true if walkable
     */
    bool IsWalkableGround() const {
        return IsGrounded() && GetGroundAngle() <= settings.slopeLimit;
    }
};

} // namespace Physics::Component
```

### CharacterMovement Resource

```cpp
/**************************************************************************
 * @file CharacterMovement.hpp
 * @brief Character movement utilities
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Character movement utilities
 * 
 * Provides functions to move character controllers based on input.
 * Works with the Input plugin for keyboard/mouse control.
 */
class CharacterMovement {
public:
    /**
     * @brief Move character based on input direction
     * @param core Engine core
     * @param entity Entity with CharacterController
     * @param inputDirection Input direction (normalized, in world space)
     * @param isRunning Whether character is running (vs walking)
     * 
     * Applies movement force based on input. Handles acceleration,
     * air control, and slope alignment.
     * 
     * @example
     * @code
     * // In update loop
     * glm::vec3 input = GetInputDirection();  // From Input plugin
     * bool running = IsRunKeyPressed();
     * CharacterMovement::Move(core, player, input, running);
     * @endcode
     */
    static void Move(Engine::Core &core, Engine::Entity entity,
                     const glm::vec3 &inputDirection, bool isRunning = false);
    
    /**
     * @brief Make character jump
     * @param core Engine core
     * @param entity Entity with CharacterController
     * @return true if jump successful
     * 
     * Makes character jump if on ground (or if double jump available).
     * Applies upward impulse based on jumpHeight setting.
     */
    static bool Jump(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Set character velocity directly
     * @param core Engine core
     * @param entity Entity with CharacterController
     * @param velocity Target velocity
     * 
     * Directly sets velocity (bypasses acceleration). Useful for
     * teleports, knockback, or scripted movement.
     */
    static void SetVelocity(Engine::Core &core, Engine::Entity entity,
                           const glm::vec3 &velocity);
    
    /**
     * @brief Apply impulse to character
     * @param core Engine core
     * @param entity Entity with CharacterController
     * @param impulse Impulse vector
     * 
     * Applies impulse (instant velocity change). Useful for knockback,
     * explosions, jump pads.
     */
    static void ApplyImpulse(Engine::Core &core, Engine::Entity entity,
                            const glm::vec3 &impulse);
    
    /**
     * @brief Teleport character (no physics simulation)
     * @param core Engine core
     * @param entity Entity with CharacterController
     * @param position Target position
     * @param resetVelocity Whether to reset velocity to zero
     */
    static void Teleport(Engine::Core &core, Engine::Entity entity,
                        const glm::vec3 &position, bool resetVelocity = true);
};

} // namespace Physics::Resource
```

### CharacterControllerSystem

```cpp
/**************************************************************************
 * @file CharacterControllerSystem.hpp
 * @brief System for managing character controllers
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::System {

/**
 * @brief Character controller system
 * 
 * Manages character controller lifecycle:
 * 1. Create Jolt character on component add
 * 2. Update ground state every frame
 * 3. Handle slope/stair logic
 * 4. Cleanup on component remove
 */
class CharacterControllerSystem {
public:
    /**
     * @brief Initialize character controller system
     * @param core Engine core
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown character controller system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Update character controllers (before physics step)
     * @param core Engine core
     * 
     * Updates ground state, handles slopes/stairs.
     */
    static void PreUpdate(Engine::Core &core);
    
    /**
     * @brief Update character controllers (after physics step)
     * @param core Engine core
     * 
     * Updates velocity state, resets jump count if grounded.
     */
    static void PostUpdate(Engine::Core &core);
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Create Component (3-4 hours)

- [ ] Create `component/CharacterController.hpp`
- [ ] Implement GroundState enum
- [ ] Implement CharacterControllerSettings struct
- [ ] Implement CharacterController struct
- [ ] Add factory methods
- [ ] Add state query methods (IsGrounded, IsSliding, etc.)
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement CharacterMovement Resource (10-12 hours)

- [ ] Create `resource/CharacterMovement.hpp`
- [ ] Implement Move()
  - Calculate target speed (walk vs run)
  - Apply acceleration (smooth start/stop)
  - Handle air control (reduced movement in air)
  - Align movement to slope
- [ ] Implement Jump()
  - Check if grounded or double jump available
  - Calculate jump velocity from jumpHeight
  - Apply upward impulse
  - Increment jump count
- [ ] Implement SetVelocity()
- [ ] Implement ApplyImpulse()
- [ ] Implement Teleport()

### Phase 3: Implement CharacterControllerSystem (12-14 hours)

- [ ] Create `system/CharacterControllerSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement PreUpdate()
  - Ground detection (raycast down)
  - Calculate ground normal and angle
  - Update GroundState (Grounded/InAir/Sliding)
  - Stair stepping (step-up logic)
- [ ] Implement PostUpdate()
  - Update velocity state
  - Reset jump count if grounded
  - Apply gravity
- [ ] Integrate with Jolt Character

### Phase 4: Unit Tests (8-10 hours)

- [ ] Create `tests/CharacterControllerTest.cpp`
- [ ] Test ground detection (ray hit/miss)
- [ ] Test slope handling (walkable/too steep)
- [ ] Test stair stepping (auto step-up)
- [ ] Test jump mechanics (single/double)
- [ ] Test air control (limited movement)
- [ ] Test movement acceleration
- [ ] Test teleportation

### Phase 5: Integration Examples (8-10 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Basic Character" example (WASD movement)
- [ ] Add "Jumping Character" example (Space to jump)
- [ ] Add "Platformer Character" example (double jump, tight control)
- [ ] Add "Slope Test" example (test walkable angles)
- [ ] Add "Stairs Test" example (auto step-up)
- [ ] Add "Push Objects" example (character pushes boxes)

### Phase 6: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Character Controller" section
  - Explain ground detection
  - Explain slope handling
  - Explain stair stepping
  - Integration with Input plugin
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Ground detection works (ray hit/miss)
- [ ] Slope detection works (angle calculation)
- [ ] Walkable slope (< slopeLimit)
- [ ] Too steep slope (sliding)
- [ ] Stair stepping works (auto step-up)
- [ ] Jump works (grounded only)
- [ ] Double jump works (if enabled)
- [ ] Air control reduces movement
- [ ] Movement acceleration works
- [ ] Teleport works

### Integration Tests

- [ ] Character walks on flat ground
- [ ] Character climbs walkable slope
- [ ] Character slides down steep slope
- [ ] Character steps up stairs
- [ ] Character jumps correctly
- [ ] Character has air control
- [ ] Character pushes dynamic objects

### Performance Tests

- [ ] 50 characters: < 3ms overhead
- [ ] Ground detection: < 0.5ms per character

### Edge Cases

- [ ] Jump while already jumping (blocked)
- [ ] Move on very steep slope (slide down)
- [ ] Teleport to invalid position
- [ ] Step too high (blocked)

## 📚 Code Examples

### Example 1: Basic Character Movement (WASD + Jump)

```cpp
// Create player character with controller
void CreatePlayer(Engine::Core &core, glm::vec3 spawnPos) {
    // Entity
    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(spawnPos));
    
    // Physics (Dynamic with locked rotation)
    auto rb = Physics::Component::RigidBody::CreateDynamic(80.0f);
    rb.lockRotationX = true;  // Prevent tipping
    rb.lockRotationZ = true;
    player.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Character controller
    auto controller = Physics::Component::CharacterController::Create(
        Physics::Component::CharacterControllerSettings::Default()
    );
    player.AddComponent<Physics::Component::CharacterController>(core, controller);
    
    // Capsule collider (character shape)
    Physics::Component::CapsuleCollider collider(1.0f, 0.5f);
    player.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
    
    Logger::Info("Player created at: {}, {}, {}", spawnPos.x, spawnPos.y, spawnPos.z);
}

// Update loop (integrate with Input plugin)
void UpdatePlayer(Engine::Core &core, Engine::Entity player) {
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    
    // Get input direction (from Input plugin)
    glm::vec3 inputDir = glm::vec3(0);
    if (inputManager.IsKeyPressed(GLFW_KEY_W)) inputDir.z += 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_S)) inputDir.z -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_A)) inputDir.x -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_D)) inputDir.x += 1.0f;
    
    // Normalize input (prevent faster diagonal movement)
    if (glm::length(inputDir) > 0.0f) {
        inputDir = glm::normalize(inputDir);
    }
    
    // Move character
    bool isRunning = inputManager.IsKeyPressed(GLFW_KEY_LEFT_SHIFT);
    Physics::Resource::CharacterMovement::Move(core, player, inputDir, isRunning);
    
    // Jump
    if (inputManager.IsKeyPressed(GLFW_KEY_SPACE)) {
        Physics::Resource::CharacterMovement::Jump(core, player);
    }
}
```

### Example 2: Platformer Character (Double Jump, High Control)

```cpp
// Create platformer character with tight controls
void CreatePlatformerCharacter(Engine::Core &core, glm::vec3 spawnPos) {
    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(spawnPos));
    
    // Lighter body for platformer feel
    auto rb = Physics::Component::RigidBody::CreateDynamic(60.0f);
    rb.lockRotationX = true;
    rb.lockRotationZ = true;
    player.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Platformer settings (high jump, double jump, tight control)
    auto controller = Physics::Component::CharacterController::Create(
        Physics::Component::CharacterControllerSettings::Platformer()
    );
    player.AddComponent<Physics::Component::CharacterController>(core, controller);
    
    // Smaller capsule for tight spaces
    Physics::Component::CapsuleCollider collider(0.8f, 0.4f);
    player.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
    
    Logger::Info("Platformer character created");
}
```

### Example 3: Check Ground State

```cpp
// Check if character can perform action
void CheckCharacterState(Engine::Core &core, Engine::Entity player) {
    auto &controller = player.GetComponent<Physics::Component::CharacterController>(core);
    
    if (controller.IsGrounded()) {
        Logger::Info("Character is on ground (can walk/jump)");
        Logger::Info("Ground angle: {}°", controller.GetGroundAngle());
        
        if (controller.IsWalkableGround()) {
            Logger::Info("Ground is walkable");
        } else {
            Logger::Info("Ground is too steep (sliding)");
        }
    } else if (controller.IsInAir()) {
        Logger::Info("Character is in air (falling/jumping)");
        Logger::Info("Velocity: {}, {}, {}",
                     controller.velocity.x,
                     controller.velocity.y,
                     controller.velocity.z);
    }
    
    if (controller.canJump) {
        Logger::Info("Can jump now");
    }
    
    Logger::Info("Jump count: {}/{}", controller.jumpCount,
                 controller.settings.canDoubleJump ? 2 : 1);
}
```

### Example 4: Slope Test Level

```cpp
// Create test level with various slopes
void CreateSlopeTestLevel(Engine::Core &core) {
    // Flat ground
    auto flat = CreateStaticBox(core, glm::vec3(0, 0, 0), glm::vec3(10, 0.5f, 10));
    
    // Walkable slope (30°)
    auto walkableSlope = CreateStaticBox(core, glm::vec3(15, 2, 0), glm::vec3(5, 0.5f, 5));
    auto &transformWalkable = walkableSlope.GetComponent<Object::Component::Transform>(core);
    transformWalkable.rotation = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 0, 1));
    
    // Steep slope (60° - should slide)
    auto steepSlope = CreateStaticBox(core, glm::vec3(25, 4, 0), glm::vec3(5, 0.5f, 5));
    auto &transformSteep = steepSlope.GetComponent<Object::Component::Transform>(core);
    transformSteep.rotation = glm::angleAxis(glm::radians(60.0f), glm::vec3(0, 0, 1));
    
    Logger::Info("Slope test level created");
}
```

### Example 5: Stairs Test

```cpp
// Create stairs to test auto step-up
void CreateStairs(Engine::Core &core, glm::vec3 startPos, int stepCount) {
    const float stepWidth = 2.0f;
    const float stepHeight = 0.3f;  // 30cm steps
    const float stepDepth = 1.0f;
    
    for (int i = 0; i < stepCount; ++i) {
        glm::vec3 stepPos = startPos + glm::vec3(
            0,
            i * stepHeight,
            i * stepDepth
        );
        
        auto step = core.CreateEntity();
        step.AddComponent<Object::Component::Transform>(core,
            Object::Component::Transform::Create(stepPos));
        
        Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
        step.AddComponent<Physics::Component::RigidBody>(core, rb);
        
        Physics::Component::BoxCollider collider(glm::vec3(stepWidth, stepHeight, stepDepth) * 0.5f);
        step.AddComponent<Physics::Component::BoxCollider>(core, collider);
    }
    
    Logger::Info("Created {} stairs", stepCount);
}
```

### Example 6: Teleport and Knockback

```cpp
// Teleport character to checkpoint
void TeleportToCheckpoint(Engine::Core &core, Engine::Entity player, glm::vec3 checkpoint) {
    Physics::Resource::CharacterMovement::Teleport(core, player, checkpoint, true);
    Logger::Info("Player teleported to checkpoint");
}

// Apply knockback (explosion, damage)
void ApplyKnockback(Engine::Core &core, Engine::Entity player, glm::vec3 direction, float force) {
    glm::vec3 impulse = glm::normalize(direction) * force;
    Physics::Resource::CharacterMovement::ApplyImpulse(core, player, impulse);
    Logger::Info("Knockback applied: force = {}", force);
}
```

## 🐛 Error Handling

### Cases to Handle

1. **No RigidBody component**
   ```cpp
   if (!entity.HasComponent<RigidBody>(core)) {
       Logger::Error("CharacterController: Entity missing RigidBody component");
       return;
   }
   ```

2. **No Collider component**
   ```cpp
   if (!entity.HasComponent<CapsuleCollider>(core) && !entity.HasComponent<BoxCollider>(core)) {
       Logger::Error("CharacterController: Entity missing Collider component");
       return;
   }
   ```

3. **Invalid input direction**
   ```cpp
   if (glm::length(inputDirection) > 1.1f) {
       Logger::Warning("CharacterMovement::Move: Input direction not normalized");
       inputDirection = glm::normalize(inputDirection);
   }
   ```

4. **Jump while not grounded**
   ```cpp
   if (!controller.IsGrounded() && controller.jumpCount >= maxJumps) {
       Logger::Debug("CharacterMovement::Jump: Cannot jump (not grounded)");
       return false;
   }
   ```

## 💡 Design Considerations

### Character Controller vs Direct RigidBody Control

| Feature | CharacterController | Direct RigidBody |
|---------|---------------------|------------------|
| **Ground detection** | Automatic | Manual raycast |
| **Slope handling** | Automatic | Manual |
| **Stair stepping** | Automatic | Manual |
| **Air control** | Built-in | Custom logic |
| **Jump mechanics** | Built-in | Custom logic |
| **Use case** | Player/NPC | Generic objects |

### Integration with Input Plugin

```cpp
// Input plugin handles keyboard/mouse
auto &inputManager = core.GetResource<Input::Resource::InputManager>();

// Get input (Input plugin responsibility)
glm::vec3 input = GetInputDirection(inputManager);

// Apply movement (Physics plugin responsibility)
Physics::Resource::CharacterMovement::Move(core, player, input, isRunning);
```

### Performance Notes

- Ground detection uses single raycast per frame (cheap)
- Stair stepping uses additional raycasts (only when near steps)
- Air control reduces movement force (no extra cost)
- Recommended: max 50-100 characters per scene

## 📊 Success Criteria

- [ ] CharacterController component implemented
- [ ] Ground detection working (raycast)
- [ ] Slope handling working (walkable angle)
- [ ] Stair stepping working (auto step-up)
- [ ] Jump mechanics working (single/double)
- [ ] Movement acceleration working
- [ ] Air control working
- [ ] CharacterMovement resource implemented
- [ ] CharacterControllerSystem implemented
- [ ] Jolt integration working
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples with Input plugin
- [ ] Performance acceptable (< 3ms for 50 characters)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Character: https://jrouwe.github.io/JoltPhysics/class_character.html
- Unity CharacterController: https://docs.unity3d.com/Manual/class-CharacterController.html
- Godot CharacterBody3D: https://docs.godotengine.org/en/stable/classes/class_characterbody3d.html
- Character Controller Theory: https://www.gdcvault.com/play/1020583/Physics-for-Game-Programmers-Character

## 🚀 Follow-up Issues

- #019: Vehicle Physics (similar physics-based control)
- #020: Debug Visualization (show ground rays, slope normals)
- #023: AI Navigation (use CharacterController for NPCs)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.7`, `priority:critical`, `character-controller`, `gameplay`  
**Milestone:** v0.7 - Advanced Physics
