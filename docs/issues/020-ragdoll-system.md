# Issue #020: Implement Ragdoll System (Procedural Character Physics)

**Milestone:** v0.7 - Advanced Physics  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 4-5 days  
**Dependencies:** #001-005 (Core Physics), #015-017 (Constraints for bone joints)  
**Related Issues:** #019 (Vehicle Physics), #021 (Soft Body Physics)  
**Follow-up Issues:** #024 (Debug Visualization), #025 (Serialization)

---

## 📋 Description

Implement procedural ragdoll system for realistic character physics. Automatically generates ragdoll constraints from character skeleton, handles death animations, active/inactive states, and provides tools for ragdoll setup.

## 🎯 Goals

- ✅ Implement Ragdoll component
- ✅ Implement RagdollBone component
- ✅ Procedural ragdoll generation from skeleton
- ✅ Active/Inactive state management
- ✅ Death animation blending
- ✅ Hit reactions (partial ragdoll)
- ✅ Ragdoll recovery (get up)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Ragdoll.hpp                 # New file
│   └── RagdollBone.hpp             # New file
├── resource/
│   ├── RagdollBuilder.hpp          # New file
│   └── RagdollBuilder.cpp          # New file
├── system/
│   ├── RagdollSystem.hpp           # New file
│   └── RagdollSystem.cpp           # New file
└── tests/
    └── RagdollTest.cpp             # New file
```

### Ragdoll Component

```cpp
/**************************************************************************
 * @file Ragdoll.hpp
 * @brief Ragdoll component (character physics)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Ragdoll state
 */
enum class RagdollState {
    Inactive,      ///< Ragdoll inactive (animated)
    Active,        ///< Ragdoll fully active (physics)
    Blending,      ///< Transitioning (anim to physics)
    PartialActive  ///< Partial ragdoll (hit reaction)
};

/**
 * @brief Bone type (for constraint setup)
 */
enum class BoneType {
    Spine,         ///< Spine/torso
    Head,          ///< Head/neck
    UpperArm,      ///< Shoulder to elbow
    LowerArm,      ///< Elbow to wrist
    Hand,          ///< Hand
    UpperLeg,      ///< Hip to knee
    LowerLeg,      ///< Knee to ankle
    Foot,          ///< Foot
    Other          ///< Generic bone
};

/**
 * @brief Ragdoll profile (preset configurations)
 */
struct RagdollProfile {
    std::string name = "Default";
    
    /// Mass distribution (kg)
    float headMass = 5.0f;
    float torsaMass = 40.0f;
    float upperArmMass = 3.0f;
    float lowerArmMass = 2.0f;
    float upperLegMass = 10.0f;
    float lowerLegMass = 5.0f;
    
    /// Constraint limits (degrees)
    float headLimit = 45.0f;
    float spineLimit = 30.0f;
    float shoulderLimit = 120.0f;
    float elbowLimit = 140.0f;
    float hipLimit = 90.0f;
    float kneeLimit = 140.0f;
    
    /// Physics
    float damping = 0.5f;           ///< Joint damping
    float selfCollision = false;    ///< Enable self-collision
    
    /**
     * @brief Default human ragdoll
     * @return RagdollProfile
     */
    static RagdollProfile Human() {
        return {};
    }
    
    /**
     * @brief Stiff ragdoll (less floppy)
     * @return RagdollProfile
     */
    static RagdollProfile Stiff() {
        RagdollProfile p;
        p.damping = 0.8f;
        p.headLimit = 30.0f;
        p.spineLimit = 20.0f;
        p.shoulderLimit = 90.0f;
        return p;
    }
    
    /**
     * @brief Loose ragdoll (very floppy)
     * @return RagdollProfile
     */
    static RagdollProfile Loose() {
        RagdollProfile p;
        p.damping = 0.2f;
        p.headLimit = 60.0f;
        p.spineLimit = 45.0f;
        p.shoulderLimit = 150.0f;
        return p;
    }
};

/**
 * @brief Ragdoll component
 * 
 * Manages ragdoll physics for a character. Contains references to
 * all ragdoll bones and handles state transitions.
 * 
 * **Workflow:**
 * 1. Create character with skeleton
 * 2. Use RagdollBuilder to generate ragdoll
 * 3. Ragdoll starts Inactive (animated)
 * 4. Activate() to enable physics
 * 5. Deactivate() to return to animation
 * 
 * @example
 * @code
 * // Create character with ragdoll
 * auto character = core.CreateEntity();
 * 
 * // ... setup character model, animation, etc ...
 * 
 * // Generate ragdoll from skeleton
 * auto ragdoll = Physics::Resource::RagdollBuilder::BuildFromSkeleton(
 *     core, character, skeletonData, Physics::Component::RagdollProfile::Human()
 * );
 * character.AddComponent<Physics::Component::Ragdoll>(core, ragdoll);
 * 
 * // Activate ragdoll on death
 * void OnCharacterDeath(Engine::Entity character) {
 *     Physics::Resource::RagdollBuilder::Activate(core, character);
 * }
 * @endcode
 */
struct Ragdoll {
    /// Profile
    RagdollProfile profile;
    
    /// Bones (ragdoll bone entities)
    std::vector<Engine::Entity> bones;
    
    /// Root bone (usually pelvis)
    Engine::Entity rootBone;
    
    /// State
    RagdollState state = RagdollState::Inactive;
    float blendTime = 0.3f;         ///< Blend duration (seconds)
    float blendProgress = 0.0f;     ///< Current blend progress [0, 1]
    
    /// Partial ragdoll (for hit reactions)
    std::vector<Engine::Entity> partialBones;  ///< Bones currently ragdolled
    
    /**
     * @brief Create ragdoll
     * @param ragdollProfile Ragdoll profile
     * @return Ragdoll
     */
    static Ragdoll Create(const RagdollProfile &ragdollProfile = RagdollProfile::Human()) {
        Ragdoll r;
        r.profile = ragdollProfile;
        return r;
    }
    
    /**
     * @brief Check if ragdoll is active
     * @return true if active
     */
    bool IsActive() const {
        return state == RagdollState::Active || state == RagdollState::Blending;
    }
    
    /**
     * @brief Check if ragdoll is inactive (animated)
     * @return true if inactive
     */
    bool IsInactive() const {
        return state == RagdollState::Inactive;
    }
    
    /**
     * @brief Check if blending
     * @return true if blending
     */
    bool IsBlending() const {
        return state == RagdollState::Blending;
    }
    
    /**
     * @brief Get bone count
     * @return Number of bones
     */
    size_t GetBoneCount() const {
        return bones.size();
    }
};

} // namespace Physics::Component
```

### RagdollBone Component

```cpp
/**************************************************************************
 * @file RagdollBone.hpp
 * @brief Ragdoll bone component (individual bone)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Ragdoll bone component
 * 
 * Represents a single bone in a ragdoll. Each bone has:
 * - RigidBody (mass, physics)
 * - Collider (capsule or box)
 * - Constraint to parent bone (6DOF with limits)
 * 
 * @example
 * @code
 * // Create ragdoll bone (usually done by RagdollBuilder)
 * auto boneEntity = core.CreateEntity();
 * 
 * auto bone = Physics::Component::RagdollBone::Create(
 *     "LeftUpperArm",
 *     Physics::Component::BoneType::UpperArm,
 *     parentBone
 * );
 * bone.mass = 3.0f;
 * bone.length = 0.3f;
 * bone.radius = 0.05f;
 * 
 * boneEntity.AddComponent<Physics::Component::RagdollBone>(core, bone);
 * @endcode
 */
struct RagdollBone {
    /// Identity
    std::string name = "";           ///< Bone name (e.g., "LeftUpperArm")
    BoneType type = BoneType::Other; ///< Bone type
    
    /// Hierarchy
    Engine::Entity parent;           ///< Parent bone entity
    Engine::Entity ragdoll;          ///< Parent ragdoll entity
    
    /// Physics properties
    float mass = 1.0f;               ///< Bone mass (kg)
    float length = 0.3f;             ///< Bone length (meters)
    float radius = 0.05f;            ///< Bone radius (meters)
    
    /// Constraint (to parent)
    Engine::Entity constraintEntity; ///< 6DOF constraint entity
    
    /// Animation blending
    glm::vec3 animatedPosition = glm::vec3(0.0f);  ///< Position from animation
    glm::quat animatedRotation = glm::quat(1, 0, 0, 0);  ///< Rotation from animation
    
    /**
     * @brief Create ragdoll bone
     * @param boneName Bone name
     * @param boneType Bone type
     * @param parentBone Parent bone entity
     * @return RagdollBone
     */
    static RagdollBone Create(
        const std::string &boneName,
        BoneType boneType,
        Engine::Entity parentBone = Engine::Entity()
    ) {
        RagdollBone b;
        b.name = boneName;
        b.type = boneType;
        b.parent = parentBone;
        return b;
    }
    
    /**
     * @brief Check if root bone (no parent)
     * @return true if root
     */
    bool IsRoot() const {
        return !parent.IsValid();
    }
};

} // namespace Physics::Component
```

### RagdollBuilder Resource

```cpp
/**************************************************************************
 * @file RagdollBuilder.hpp
 * @brief Ragdoll builder utilities
 **************************************************************************/

#pragma once

#include <vector>
#include <string>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {
    struct RagdollProfile;
}

namespace Physics::Resource {

/**
 * @brief Skeleton bone data (for ragdoll generation)
 */
struct SkeletonBone {
    std::string name;              ///< Bone name
    glm::vec3 position;            ///< Local position
    glm::quat rotation;            ///< Local rotation
    int parentIndex = -1;          ///< Parent bone index (-1 = root)
};

/**
 * @brief Ragdoll builder
 * 
 * Utilities to procedurally generate ragdolls from character skeletons.
 */
class RagdollBuilder {
public:
    /**
     * @brief Build ragdoll from skeleton
     * @param core Engine core
     * @param character Character entity
     * @param skeleton Skeleton bone data
     * @param profile Ragdoll profile
     * @return Ragdoll component
     * 
     * Automatically creates:
     * - RagdollBone entities for each skeleton bone
     * - RigidBody components (with mass from profile)
     * - CapsuleCollider components (auto-sized)
     * - 6DOF constraints between bones (with limits from profile)
     */
    static Component::Ragdoll BuildFromSkeleton(
        Engine::Core &core,
        Engine::Entity character,
        const std::vector<SkeletonBone> &skeleton,
        const Component::RagdollProfile &profile = Component::RagdollProfile::Human()
    );
    
    /**
     * @brief Activate ragdoll (switch to physics)
     * @param core Engine core
     * @param character Character entity with Ragdoll component
     * @param impulse Optional impulse to apply (e.g., death force)
     * 
     * Transitions from animation to physics:
     * 1. Enable all bone RigidBodies
     * 2. Copy current animated pose to physics
     * 3. Apply impulse (if provided)
     * 4. Start blend timer
     */
    static void Activate(Engine::Core &core, Engine::Entity character,
                         const glm::vec3 &impulse = glm::vec3(0.0f));
    
    /**
     * @brief Deactivate ragdoll (switch to animation)
     * @param core Engine core
     * @param character Character entity with Ragdoll component
     * 
     * Transitions from physics to animation:
     * 1. Disable all bone RigidBodies
     * 2. Start blend timer (smooth transition)
     */
    static void Deactivate(Engine::Core &core, Engine::Entity character);
    
    /**
     * @brief Activate partial ragdoll (hit reaction)
     * @param core Engine core
     * @param character Character entity
     * @param boneNames Bones to ragdoll (e.g., {"LeftUpperArm", "LeftLowerArm"})
     * @param impulse Impulse to apply
     * 
     * Activates only specified bones for hit reactions.
     * Rest of body stays animated.
     */
    static void ActivatePartial(Engine::Core &core, Engine::Entity character,
                                 const std::vector<std::string> &boneNames,
                                 const glm::vec3 &impulse);
    
    /**
     * @brief Apply impulse to specific bone
     * @param core Engine core
     * @param character Character entity
     * @param boneName Bone name
     * @param impulse Impulse vector
     */
    static void ApplyImpulseToBone(Engine::Core &core, Engine::Entity character,
                                   const std::string &boneName, const glm::vec3 &impulse);
    
    /**
     * @brief Get bone entity by name
     * @param core Engine core
     * @param character Character entity
     * @param boneName Bone name
     * @return Bone entity (invalid if not found)
     */
    static Engine::Entity GetBone(Engine::Core &core, Engine::Entity character,
                                  const std::string &boneName);
};

} // namespace Physics::Resource
```

### RagdollSystem

```cpp
/**************************************************************************
 * @file RagdollSystem.hpp
 * @brief System for managing ragdolls
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
}

namespace Physics::System {

/**
 * @brief Ragdoll system
 * 
 * Manages ragdoll lifecycle:
 * 1. Update blending (anim <-> physics)
 * 2. Sync bone transforms
 * 3. Handle state transitions
 */
class RagdollSystem {
public:
    /**
     * @brief Initialize ragdoll system
     * @param core Engine core
     */
    static void Initialize(Engine::Core &core);
    
    /**
     * @brief Shutdown ragdoll system
     * @param core Engine core
     */
    static void Shutdown(Engine::Core &core);
    
    /**
     * @brief Update ragdolls
     * @param core Engine core
     * @param deltaTime Delta time (seconds)
     * 
     * Updates:
     * - Blend progress
     * - Bone transforms (anim -> physics or physics -> visual)
     * - State transitions
     */
    static void Update(Engine::Core &core, float deltaTime);
};

} // namespace Physics::System
```

## 📝 Implementation Tasks

### Phase 1: Create Components (3-4 hours)

- [ ] Create `component/Ragdoll.hpp`
- [ ] Implement RagdollState enum
- [ ] Implement BoneType enum
- [ ] Implement RagdollProfile struct
- [ ] Implement Ragdoll component
- [ ] Create `component/RagdollBone.hpp`
- [ ] Implement RagdollBone component
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement RagdollBuilder (12-14 hours)

- [ ] Create `resource/RagdollBuilder.hpp`
- [ ] Implement BuildFromSkeleton()
  - Parse skeleton hierarchy
  - Create bone entities
  - Assign masses based on bone type
  - Create capsule colliders (auto-sized)
  - Create 6DOF constraints with limits
  - Setup constraint hierarchy
- [ ] Implement Activate()
  - Enable RigidBodies
  - Copy animated pose to physics
  - Apply impulse
- [ ] Implement Deactivate()
  - Disable RigidBodies
  - Start blend
- [ ] Implement ActivatePartial()
- [ ] Implement ApplyImpulseToBone()
- [ ] Implement GetBone()

### Phase 3: Implement RagdollSystem (8-10 hours)

- [ ] Create `system/RagdollSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement Update()
  - Update blend progress
  - Blend bone transforms (anim <-> physics)
  - Sync visual transforms
  - Handle state transitions

### Phase 4: Unit Tests (8-10 hours)

- [ ] Create `tests/RagdollTest.cpp`
- [ ] Test ragdoll generation from skeleton
- [ ] Test activate/deactivate
- [ ] Test blending (smooth transition)
- [ ] Test partial ragdoll
- [ ] Test impulse application
- [ ] Test bone hierarchy

### Phase 5: Integration Examples (8-10 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Basic Ragdoll" example (death animation)
- [ ] Add "Hit Reaction" example (partial ragdoll)
- [ ] Add "Ragdoll Profiles" example (stiff vs loose)
- [ ] Add "Ragdoll Recovery" example (get up)
- [ ] Add "Ragdoll Impulse" example (push/explosion)

### Phase 6: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Ragdoll System" section
  - Explain ragdoll generation
  - Explain state management
  - Explain blending
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Ragdoll generates from skeleton
- [ ] Constraints created correctly
- [ ] Activate switches to physics
- [ ] Deactivate switches to animation
- [ ] Blending works smoothly
- [ ] Partial ragdoll works
- [ ] Impulse applies correctly
- [ ] Bone lookup works

### Integration Tests

- [ ] Character dies (ragdoll activates)
- [ ] Ragdoll falls realistically
- [ ] Hit reaction (arm flinches)
- [ ] Ragdoll recovery (get up)
- [ ] Different profiles behave differently

### Performance Tests

- [ ] 10 ragdolls: < 5ms overhead
- [ ] Blending: < 1ms per ragdoll

### Edge Cases

- [ ] Activate already active ragdoll
- [ ] Deactivate inactive ragdoll
- [ ] Partial ragdoll with invalid bones
- [ ] Very large impulse (extreme force)

## 📚 Code Examples

### Example 1: Create Character with Ragdoll

```cpp
// Create character with procedural ragdoll
Engine::Entity CreateCharacter(Engine::Core &core, glm::vec3 spawnPos) {
    // Character entity
    auto character = core.CreateEntity();
    character.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(spawnPos));
    
    // ... Setup character model, animation system, etc ...
    
    // Define skeleton (simplified humanoid)
    std::vector<Physics::Resource::SkeletonBone> skeleton = {
        {"Pelvis", glm::vec3(0, 1, 0), glm::quat(1, 0, 0, 0), -1},  // Root
        {"Spine", glm::vec3(0, 1.3f, 0), glm::quat(1, 0, 0, 0), 0},
        {"Head", glm::vec3(0, 1.7f, 0), glm::quat(1, 0, 0, 0), 1},
        
        // Left arm
        {"LeftShoulder", glm::vec3(0.2f, 1.5f, 0), glm::quat(1, 0, 0, 0), 1},
        {"LeftUpperArm", glm::vec3(0.5f, 1.5f, 0), glm::quat(1, 0, 0, 0), 3},
        {"LeftLowerArm", glm::vec3(0.8f, 1.5f, 0), glm::quat(1, 0, 0, 0), 4},
        
        // Right arm
        {"RightShoulder", glm::vec3(-0.2f, 1.5f, 0), glm::quat(1, 0, 0, 0), 1},
        {"RightUpperArm", glm::vec3(-0.5f, 1.5f, 0), glm::quat(1, 0, 0, 0), 6},
        {"RightLowerArm", glm::vec3(-0.8f, 1.5f, 0), glm::quat(1, 0, 0, 0), 7},
        
        // Left leg
        {"LeftUpperLeg", glm::vec3(0.1f, 0.5f, 0), glm::quat(1, 0, 0, 0), 0},
        {"LeftLowerLeg", glm::vec3(0.1f, 0.0f, 0), glm::quat(1, 0, 0, 0), 9},
        
        // Right leg
        {"RightUpperLeg", glm::vec3(-0.1f, 0.5f, 0), glm::quat(1, 0, 0, 0), 0},
        {"RightLowerLeg", glm::vec3(-0.1f, 0.0f, 0), glm::quat(1, 0, 0, 0), 11},
    };
    
    // Generate ragdoll
    auto ragdoll = Physics::Resource::RagdollBuilder::BuildFromSkeleton(
        core, character, skeleton, Physics::Component::RagdollProfile::Human()
    );
    character.AddComponent<Physics::Component::Ragdoll>(core, ragdoll);
    
    Logger::Info("Character created with {} bone ragdoll", ragdoll.GetBoneCount());
    return character;
}
```

### Example 2: Character Death (Activate Ragdoll)

```cpp
// Kill character (activate ragdoll physics)
void KillCharacter(Engine::Core &core, Engine::Entity character, glm::vec3 deathForce) {
    auto &ragdoll = character.GetComponent<Physics::Component::Ragdoll>(core);
    
    if (ragdoll.IsInactive()) {
        // Activate ragdoll with death impulse
        Physics::Resource::RagdollBuilder::Activate(core, character, deathForce);
        
        Logger::Info("Character died (ragdoll activated)");
        
        // Play death sound, spawn effects, etc.
        Audio::PlaySound("sounds/death.wav");
        Particles::SpawnBlood(core, GetCharacterPosition(core, character));
    }
}
```

### Example 3: Hit Reaction (Partial Ragdoll)

```cpp
// Character hit (partial ragdoll on hit limb)
void OnCharacterHit(Engine::Core &core, Engine::Entity character,
                   const std::string &hitBone, glm::vec3 hitForce) {
    // Activate only hit limb (e.g., left arm)
    std::vector<std::string> limbBones;
    
    if (hitBone == "LeftUpperArm") {
        limbBones = {"LeftUpperArm", "LeftLowerArm"};
    } else if (hitBone == "RightUpperArm") {
        limbBones = {"RightUpperArm", "RightLowerArm"};
    } else if (hitBone == "Head") {
        limbBones = {"Head"};
    }
    
    // Partial ragdoll with impulse
    Physics::Resource::RagdollBuilder::ActivatePartial(core, character, limbBones, hitForce);
    
    Logger::Info("Hit reaction on {}", hitBone);
    
    // Auto-recover after 0.5 seconds
    Timer::SetTimeout(500, [&core, character]() {
        Physics::Resource::RagdollBuilder::Deactivate(core, character);
    });
}
```

### Example 4: Ragdoll Profiles Comparison

```cpp
// Test different ragdoll profiles
void TestRagdollProfiles(Engine::Core &core) {
    // Stiff ragdoll (less floppy)
    auto characterStiff = CreateCharacterWithProfile(core, glm::vec3(-5, 5, 0),
        Physics::Component::RagdollProfile::Stiff());
    
    // Default ragdoll
    auto characterDefault = CreateCharacterWithProfile(core, glm::vec3(0, 5, 0),
        Physics::Component::RagdollProfile::Human());
    
    // Loose ragdoll (very floppy)
    auto characterLoose = CreateCharacterWithProfile(core, glm::vec3(5, 5, 0),
        Physics::Component::RagdollProfile::Loose());
    
    // Kill all (compare ragdoll behavior)
    Physics::Resource::RagdollBuilder::Activate(core, characterStiff);
    Physics::Resource::RagdollBuilder::Activate(core, characterDefault);
    Physics::Resource::RagdollBuilder::Activate(core, characterLoose);
    
    Logger::Info("Ragdoll profiles comparison: Stiff vs Default vs Loose");
}
```

### Example 5: Explosion Impact (Apply Impulse to Bones)

```cpp
// Explosion affects ragdoll
void ApplyExplosionToRagdoll(Engine::Core &core, Engine::Entity character,
                             glm::vec3 explosionPos, float explosionForce) {
    auto &ragdoll = character.GetComponent<Physics::Component::Ragdoll>(core);
    
    // Activate ragdoll if not already
    if (ragdoll.IsInactive()) {
        Physics::Resource::RagdollBuilder::Activate(core, character);
    }
    
    // Apply impulse to each bone (proportional to distance)
    for (auto boneEntity : ragdoll.bones) {
        auto &transform = boneEntity.GetComponent<Object::Component::Transform>(core);
        
        glm::vec3 direction = transform.position - explosionPos;
        float distance = glm::length(direction);
        
        if (distance < 10.0f) {  // Within explosion radius
            direction = glm::normalize(direction);
            float force = explosionForce * (1.0f - distance / 10.0f);  // Falloff
            
            auto &bone = boneEntity.GetComponent<Physics::Component::RagdollBone>(core);
            Physics::Resource::RagdollBuilder::ApplyImpulseToBone(
                core, character, bone.name, direction * force
            );
        }
    }
    
    Logger::Info("Explosion applied to ragdoll (force: {}N)", explosionForce);
}
```

### Example 6: Ragdoll Recovery (Get Up)

```cpp
// Ragdoll recovery (get up from ground)
void RecoverRagdoll(Engine::Core &core, Engine::Entity character) {
    auto &ragdoll = character.GetComponent<Physics::Component::Ragdoll>(core);
    
    if (ragdoll.IsActive()) {
        // Deactivate ragdoll (blend back to animation)
        Physics::Resource::RagdollBuilder::Deactivate(core, character);
        
        Logger::Info("Character recovering (ragdoll -> animation)");
        
        // Play get-up animation
        // Animation::Play(character, "GetUp");
    }
}

// Auto-recover after timeout
void AutoRecover(Engine::Core &core, Engine::Entity character, float timeout = 3.0f) {
    Timer::SetTimeout(timeout * 1000, [&core, character]() {
        RecoverRagdoll(core, character);
    });
}
```

## 🐛 Error Handling

### Cases to Handle

1. **No skeleton data**
   ```cpp
   if (skeleton.empty()) {
       Logger::Error("RagdollBuilder: Empty skeleton");
       return;
   }
   ```

2. **Invalid bone hierarchy**
   ```cpp
   if (bone.parentIndex >= static_cast<int>(skeleton.size())) {
       Logger::Error("RagdollBuilder: Invalid parent index {}", bone.parentIndex);
       return;
   }
   ```

3. **Activate already active ragdoll**
   ```cpp
   if (ragdoll.IsActive()) {
       Logger::Warning("Ragdoll already active");
       return;
   }
   ```

4. **Bone not found**
   ```cpp
   auto bone = GetBone(core, character, boneName);
   if (!bone.IsValid()) {
       Logger::Error("Bone not found: {}", boneName);
       return;
   }
   ```

## 💡 Design Considerations

### Ragdoll vs Animation

```
Inactive (Animated)
  ↓
Activate() + Blend
  ↓
Active (Physics)
  ↓
Deactivate() + Blend
  ↓
Inactive (Animated)
```

### Blending Strategy

```cpp
// Linear blend between animation and physics
glm::vec3 finalPos = glm::mix(animatedPos, physicsPos, blendProgress);
glm::quat finalRot = glm::slerp(animatedRot, physicsRot, blendProgress);
```

### Performance Notes

- Each bone is a RigidBody + Collider + Constraint
- ~13 bones per humanoid = ~13 RigidBodies
- Recommended: max 20 active ragdolls per scene
- Use LOD: distant ragdolls can be simplified

## 📊 Success Criteria

- [ ] Ragdoll component implemented
- [ ] RagdollBone component implemented
- [ ] RagdollBuilder implemented
- [ ] Procedural generation from skeleton working
- [ ] Activate/Deactivate working
- [ ] Blending smooth (no popping)
- [ ] Partial ragdoll working
- [ ] Impulse application working
- [ ] RagdollSystem implemented
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 10 ragdolls)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Ragdoll: https://jrouwe.github.io/JoltPhysics/class_ragdoll.html
- Unity Ragdoll: https://docs.unity3d.com/Manual/wizard-RagdollWizard.html
- Godot PhysicalBone: https://docs.godotengine.org/en/stable/classes/class_physicalbone3d.html
- Ragdoll Physics: https://www.gdcvault.com/play/1020583/Physics-for-Game-Programmers-Networking

## 🚀 Follow-up Issues

- #023: Animation Integration (blend ragdoll with animation system)
- #020: Debug Visualization (draw ragdoll constraints)
- #024: Ragdoll Optimization (LOD, sleeping)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.7`, `priority:high`, `ragdoll`, `animation`  
**Milestone:** v0.7 - Advanced Physics
