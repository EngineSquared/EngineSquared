# Issue #021: Implement Soft Body Physics (Cloth, Ropes, Deformables)

**Milestone:** v0.7 - Advanced Physics  
**Priority:** 🟢 MEDIUM  
**Estimated Effort:** 5-6 days  
**Dependencies:** #001-005 (Core Physics), #015 (Distance Constraints for springs)  
**Related Issues:** #020 (Ragdoll System)  
**Follow-up Issues:** #024 (Debug Visualization)

---

## 📋 Description

Implement soft body physics for deformable objects: cloth simulation, ropes, soft bodies, and inflatable objects. Uses mass-spring system with constraints for realistic deformation and interaction.

## 🎯 Goals

- ✅ Implement SoftBody component
- ✅ Implement Cloth simulation (flags, capes)
- ✅ Implement Rope/Cable simulation
- ✅ Implement Volume preservation (inflatables)
- ✅ Wind force support
- ✅ Tearing/cutting support
- ✅ Attachment to rigid bodies
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── SoftBody.hpp                # New file
│   ├── Cloth.hpp                   # New file
│   └── Rope.hpp                    # New file
├── resource/
│   ├── SoftBodyBuilder.hpp         # New file
│   └── SoftBodyBuilder.cpp         # New file
├── system/
│   ├── SoftBodySystem.hpp          # New file
│   └── SoftBodySystem.cpp          # New file
└── tests/
    └── SoftBodyTest.cpp            # New file
```

### SoftBody Component

```cpp
/**************************************************************************
 * @file SoftBody.hpp
 * @brief Soft body component (deformable objects)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Soft body particle (mass point)
 */
struct SoftBodyParticle {
    glm::vec3 position = glm::vec3(0.0f);     ///< Current position
    glm::vec3 velocity = glm::vec3(0.0f);     ///< Current velocity
    glm::vec3 force = glm::vec3(0.0f);        ///< Accumulated force
    float mass = 1.0f;                        ///< Particle mass
    float invMass = 1.0f;                     ///< 1 / mass
    bool fixed = false;                       ///< Fixed in space (pinned)
    
    /**
     * @brief Create particle
     * @param pos Initial position
     * @param particleMass Mass
     * @return SoftBodyParticle
     */
    static SoftBodyParticle Create(const glm::vec3 &pos, float particleMass = 1.0f) {
        SoftBodyParticle p;
        p.position = pos;
        p.mass = particleMass;
        p.invMass = particleMass > 0.0f ? 1.0f / particleMass : 0.0f;
        return p;
    }
    
    /**
     * @brief Pin particle (fix in space)
     */
    void Pin() {
        fixed = true;
        invMass = 0.0f;
        velocity = glm::vec3(0.0f);
    }
    
    /**
     * @brief Unpin particle
     */
    void Unpin() {
        fixed = false;
        invMass = mass > 0.0f ? 1.0f / mass : 0.0f;
    }
};

/**
 * @brief Soft body constraint (spring/distance)
 */
struct SoftBodyConstraint {
    int particleA = 0;                        ///< First particle index
    int particleB = 0;                        ///< Second particle index
    float restLength = 1.0f;                  ///< Rest length
    float stiffness = 1.0f;                   ///< Spring stiffness [0, 1]
    float damping = 0.1f;                     ///< Damping [0, 1]
    
    /**
     * @brief Create constraint
     * @param a Particle A index
     * @param b Particle B index
     * @param length Rest length
     * @param stiff Stiffness [0, 1]
     * @return SoftBodyConstraint
     */
    static SoftBodyConstraint Create(int a, int b, float length, float stiff = 1.0f) {
        SoftBodyConstraint c;
        c.particleA = a;
        c.particleB = b;
        c.restLength = length;
        c.stiffness = stiff;
        return c;
    }
};

/**
 * @brief Soft body settings
 */
struct SoftBodySettings {
    /// Simulation
    float particleMass = 1.0f;                ///< Particle mass (kg)
    float damping = 0.1f;                     ///< Global damping [0, 1]
    float friction = 0.5f;                    ///< Friction coefficient
    
    /// Constraints
    float stiffness = 0.8f;                   ///< Constraint stiffness [0, 1]
    int solverIterations = 5;                 ///< Solver iterations per frame
    
    /// Wind
    bool enableWind = false;                  ///< Enable wind force
    glm::vec3 windDirection = glm::vec3(1, 0, 0);  ///< Wind direction
    float windStrength = 5.0f;                ///< Wind strength
    
    /// Collision
    bool selfCollision = false;               ///< Enable self-collision
    float collisionRadius = 0.05f;            ///< Particle collision radius
    
    /**
     * @brief Create default settings
     * @return SoftBodySettings
     */
    static SoftBodySettings Default() {
        return {};
    }
    
    /**
     * @brief Create cloth settings (flexible, wind-affected)
     * @return SoftBodySettings
     */
    static SoftBodySettings Cloth() {
        SoftBodySettings s;
        s.particleMass = 0.1f;
        s.stiffness = 0.6f;
        s.damping = 0.2f;
        s.enableWind = true;
        return s;
    }
    
    /**
     * @brief Create rope settings (strong, no wind)
     * @return SoftBodySettings
     */
    static SoftBodySettings Rope() {
        SoftBodySettings s;
        s.particleMass = 0.5f;
        s.stiffness = 0.9f;
        s.damping = 0.3f;
        s.enableWind = false;
        return s;
    }
};

/**
 * @brief Soft body component
 * 
 * Represents a deformable object using mass-spring system.
 * Particles connected by constraints form flexible structures.
 * 
 * **Common uses:**
 * - Cloth (flags, capes, curtains)
 * - Ropes (cables, chains)
 * - Soft bodies (rubber, jello)
 * - Inflatables (balloons, airbags)
 * 
 * @example
 * @code
 * // Create cloth (flag)
 * auto cloth = Physics::Resource::SoftBodyBuilder::CreateCloth(
 *     core, glm::vec3(0, 5, 0), glm::vec2(2, 2), glm::ivec2(10, 10),
 *     Physics::Component::SoftBodySettings::Cloth()
 * );
 * 
 * // Pin top corners (attach to pole)
 * cloth.GetComponent<Physics::Component::SoftBody>(core).particles[0].Pin();
 * cloth.GetComponent<Physics::Component::SoftBody>(core).particles[9].Pin();
 * @endcode
 */
struct SoftBody {
    /// Settings
    SoftBodySettings settings;
    
    /// Particles
    std::vector<SoftBodyParticle> particles;
    
    /// Constraints (springs)
    std::vector<SoftBodyConstraint> constraints;
    
    /// Attachments (to rigid bodies)
    std::vector<std::pair<int, Engine::Entity>> attachments;  ///< (particle index, entity)
    
    /// Internal (Jolt)
    uint32_t joltSoftBodyId = 0;
    
    /**
     * @brief Create soft body
     * @param softBodySettings Settings
     * @return SoftBody
     */
    static SoftBody Create(const SoftBodySettings &softBodySettings = SoftBodySettings::Default()) {
        SoftBody sb;
        sb.settings = softBodySettings;
        return sb;
    }
    
    /**
     * @brief Get particle count
     * @return Number of particles
     */
    size_t GetParticleCount() const {
        return particles.size();
    }
    
    /**
     * @brief Get constraint count
     * @return Number of constraints
     */
    size_t GetConstraintCount() const {
        return constraints.size();
    }
    
    /**
     * @brief Pin particle (fix in space)
     * @param index Particle index
     */
    void PinParticle(int index) {
        if (index >= 0 && index < static_cast<int>(particles.size())) {
            particles[index].Pin();
        }
    }
    
    /**
     * @brief Unpin particle
     * @param index Particle index
     */
    void UnpinParticle(int index) {
        if (index >= 0 && index < static_cast<int>(particles.size())) {
            particles[index].Unpin();
        }
    }
    
    /**
     * @brief Attach particle to rigid body
     * @param particleIndex Particle index
     * @param entity Rigid body entity
     */
    void AttachParticle(int particleIndex, Engine::Entity entity) {
        attachments.push_back({particleIndex, entity});
    }
};

} // namespace Physics::Component
```

### Cloth Component

```cpp
/**************************************************************************
 * @file Cloth.hpp
 * @brief Cloth component (2D grid soft body)
 **************************************************************************/

#pragma once

#include "SoftBody.hpp"
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Cloth component
 * 
 * Specialized soft body for cloth simulation (2D grid).
 * 
 * @example
 * @code
 * // Create flag (10x10 grid)
 * auto flag = Physics::Resource::SoftBodyBuilder::CreateCloth(
 *     core, glm::vec3(0, 5, 0), glm::vec2(2, 2), glm::ivec2(10, 10)
 * );
 * @endcode
 */
struct Cloth : public SoftBody {
    glm::ivec2 gridSize = glm::ivec2(10, 10);  ///< Grid dimensions (particles)
    glm::vec2 dimensions = glm::vec2(1, 1);    ///< Physical size (meters)
    
    /**
     * @brief Create cloth
     * @param size Grid size (particles)
     * @param dims Physical dimensions (meters)
     * @param settings Soft body settings
     * @return Cloth
     */
    static Cloth Create(
        const glm::ivec2 &size,
        const glm::vec2 &dims,
        const SoftBodySettings &settings = SoftBodySettings::Cloth()
    ) {
        Cloth c;
        c.gridSize = size;
        c.dimensions = dims;
        c.settings = settings;
        return c;
    }
    
    /**
     * @brief Get particle index from 2D coordinates
     * @param x X coordinate [0, gridSize.x)
     * @param y Y coordinate [0, gridSize.y)
     * @return Particle index
     */
    int GetParticleIndex(int x, int y) const {
        return y * gridSize.x + x;
    }
    
    /**
     * @brief Pin row (e.g., top edge)
     * @param row Row index [0, gridSize.y)
     */
    void PinRow(int row) {
        for (int x = 0; x < gridSize.x; ++x) {
            PinParticle(GetParticleIndex(x, row));
        }
    }
    
    /**
     * @brief Pin column (e.g., left edge)
     * @param col Column index [0, gridSize.x)
     */
    void PinColumn(int col) {
        for (int y = 0; y < gridSize.y; ++y) {
            PinParticle(GetParticleIndex(col, y));
        }
    }
};

} // namespace Physics::Component
```

### Rope Component

```cpp
/**************************************************************************
 * @file Rope.hpp
 * @brief Rope component (1D chain soft body)
 **************************************************************************/

#pragma once

#include "SoftBody.hpp"

namespace Physics::Component {

/**
 * @brief Rope component
 * 
 * Specialized soft body for rope simulation (1D chain).
 * 
 * @example
 * @code
 * // Create rope (20 segments, 5m long)
 * auto rope = Physics::Resource::SoftBodyBuilder::CreateRope(
 *     core, glm::vec3(0, 5, 0), glm::vec3(0, 0, 0), 5.0f, 20
 * );
 * 
 * // Pin both ends
 * rope.GetComponent<Physics::Component::Rope>(core).PinEnd(true);
 * rope.GetComponent<Physics::Component::Rope>(core).PinEnd(false);
 * @endcode
 */
struct Rope : public SoftBody {
    int segmentCount = 10;                     ///< Number of segments
    float length = 1.0f;                       ///< Total length (meters)
    
    /**
     * @brief Create rope
     * @param segments Number of segments
     * @param ropeLength Total length (meters)
     * @param settings Soft body settings
     * @return Rope
     */
    static Rope Create(
        int segments,
        float ropeLength,
        const SoftBodySettings &settings = SoftBodySettings::Rope()
    ) {
        Rope r;
        r.segmentCount = segments;
        r.length = ropeLength;
        r.settings = settings;
        return r;
    }
    
    /**
     * @brief Pin rope end
     * @param start true = start, false = end
     */
    void PinEnd(bool start) {
        if (start) {
            PinParticle(0);
        } else {
            PinParticle(static_cast<int>(particles.size()) - 1);
        }
    }
    
    /**
     * @brief Get segment length
     * @return Length per segment (meters)
     */
    float GetSegmentLength() const {
        return length / static_cast<float>(segmentCount);
    }
};

} // namespace Physics::Component
```

### SoftBodyBuilder Resource

```cpp
/**************************************************************************
 * @file SoftBodyBuilder.hpp
 * @brief Soft body builder utilities
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Component {
    struct SoftBodySettings;
}

namespace Physics::Resource {

/**
 * @brief Soft body builder
 * 
 * Utilities to create soft bodies (cloth, ropes, etc.)
 */
class SoftBodyBuilder {
public:
    /**
     * @brief Create cloth (2D grid)
     * @param core Engine core
     * @param position World position (top-left corner)
     * @param dimensions Physical size (meters)
     * @param gridSize Grid resolution (particles)
     * @param settings Soft body settings
     * @return Entity with Cloth component
     * 
     * Creates:
     * - Particles in grid pattern
     * - Structural constraints (horizontal + vertical)
     * - Shear constraints (diagonals)
     * - Bending constraints (skip-one)
     */
    static Engine::Entity CreateCloth(
        Engine::Core &core,
        const glm::vec3 &position,
        const glm::vec2 &dimensions,
        const glm::ivec2 &gridSize,
        const Component::SoftBodySettings &settings = Component::SoftBodySettings::Cloth()
    );
    
    /**
     * @brief Create rope (1D chain)
     * @param core Engine core
     * @param start Start position
     * @param end End position
     * @param length Total length (meters)
     * @param segments Number of segments
     * @param settings Soft body settings
     * @return Entity with Rope component
     * 
     * Creates:
     * - Particles along line from start to end
     * - Distance constraints between adjacent particles
     */
    static Engine::Entity CreateRope(
        Engine::Core &core,
        const glm::vec3 &start,
        const glm::vec3 &end,
        float length,
        int segments,
        const Component::SoftBodySettings &settings = Component::SoftBodySettings::Rope()
    );
    
    /**
     * @brief Create soft body cube (3D grid)
     * @param core Engine core
     * @param center Center position
     * @param size Cube size (meters)
     * @param gridSize Grid resolution (particles per axis)
     * @param settings Soft body settings
     * @return Entity with SoftBody component
     */
    static Engine::Entity CreateSoftCube(
        Engine::Core &core,
        const glm::vec3 &center,
        float size,
        int gridSize,
        const Component::SoftBodySettings &settings = Component::SoftBodySettings::Default()
    );
    
    /**
     * @brief Apply wind force to soft body
     * @param core Engine core
     * @param entity Soft body entity
     * @param windDir Wind direction
     * @param windStrength Wind strength
     */
    static void ApplyWind(Engine::Core &core, Engine::Entity entity,
                          const glm::vec3 &windDir, float windStrength);
    
    /**
     * @brief Cut/tear soft body
     * @param core Engine core
     * @param entity Soft body entity
     * @param position Cut position (world space)
     * @param radius Cut radius
     * 
     * Removes constraints within radius of cut position.
     */
    static void Cut(Engine::Core &core, Engine::Entity entity,
                    const glm::vec3 &position, float radius);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create Components (4-5 hours)

- [ ] Create `component/SoftBody.hpp`
- [ ] Implement SoftBodyParticle struct
- [ ] Implement SoftBodyConstraint struct
- [ ] Implement SoftBodySettings struct
- [ ] Implement SoftBody component
- [ ] Create `component/Cloth.hpp`
- [ ] Implement Cloth component (2D grid)
- [ ] Create `component/Rope.hpp`
- [ ] Implement Rope component (1D chain)
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement SoftBodyBuilder (14-16 hours)

- [ ] Create `resource/SoftBodyBuilder.hpp`
- [ ] Implement CreateCloth()
  - Generate particles in grid
  - Create structural constraints (horizontal + vertical)
  - Create shear constraints (diagonals)
  - Create bending constraints (skip-one)
- [ ] Implement CreateRope()
  - Generate particles along line
  - Create distance constraints
- [ ] Implement CreateSoftCube()
  - Generate 3D particle grid
  - Create volumetric constraints
- [ ] Implement ApplyWind()
- [ ] Implement Cut()

### Phase 3: Implement SoftBodySystem (12-14 hours)

- [ ] Create `system/SoftBodySystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement Update()
  - Integrate forces (gravity, wind)
  - Solve constraints (iterative)
  - Update velocities
  - Collision detection
  - Update visual mesh
- [ ] Integrate with Jolt Soft Body
- [ ] Handle attachments to rigid bodies

### Phase 4: Unit Tests (10-12 hours)

- [ ] Create `tests/SoftBodyTest.cpp`
- [ ] Test cloth generation
- [ ] Test rope generation
- [ ] Test constraint solving
- [ ] Test wind force
- [ ] Test pinned particles
- [ ] Test cutting/tearing
- [ ] Test attachments

### Phase 5: Integration Examples (12-14 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Flag" example (cloth with wind)
- [ ] Add "Curtain" example (cloth pinned at top)
- [ ] Add "Rope Bridge" example (rope between two points)
- [ ] Add "Hanging Rope" example (rope with weight)
- [ ] Add "Soft Cube" example (jello)
- [ ] Add "Cloth Cutting" example (tear cloth)

### Phase 6: Documentation (5-6 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Soft Body Physics" section
  - Explain cloth simulation
  - Explain rope simulation
  - Explain constraint solving
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Cloth generates correctly
- [ ] Rope generates correctly
- [ ] Constraints solve correctly
- [ ] Wind applies force
- [ ] Pinned particles stay fixed
- [ ] Cutting removes constraints
- [ ] Attachments work

### Integration Tests

- [ ] Flag waves in wind
- [ ] Curtain hangs naturally
- [ ] Rope swings realistically
- [ ] Soft cube deforms
- [ ] Cloth tears correctly

### Performance Tests

- [ ] 1000 particles: < 5ms per frame
- [ ] 10 cloths (100 particles each): < 10ms

### Edge Cases

- [ ] All particles pinned (static)
- [ ] Very high stiffness (rigid)
- [ ] Very low stiffness (floppy)
- [ ] Cut all constraints (particles fall)

## 📚 Code Examples

### Example 1: Create Flag (Cloth with Wind)

```cpp
// Create flag waving in wind
Engine::Entity CreateFlag(Engine::Core &core, glm::vec3 polePos) {
    // Create cloth (2m x 1m, 20x10 grid)
    auto flag = Physics::Resource::SoftBodyBuilder::CreateCloth(
        core,
        polePos + glm::vec3(0, 3, 0),  // Top of pole
        glm::vec2(2, 1),                // 2m wide, 1m tall
        glm::ivec2(20, 10),             // 20x10 particles
        Physics::Component::SoftBodySettings::Cloth()
    );
    
    // Pin left edge (attach to pole)
    auto &cloth = flag.GetComponent<Physics::Component::Cloth>(core);
    cloth.PinColumn(0);
    
    // Enable wind
    cloth.settings.enableWind = true;
    cloth.settings.windDirection = glm::vec3(1, 0, 0);
    cloth.settings.windStrength = 10.0f;
    
    Logger::Info("Flag created ({}x{} particles)", cloth.gridSize.x, cloth.gridSize.y);
    return flag;
}
```

### Example 2: Create Curtain

```cpp
// Create curtain (cloth pinned at top)
Engine::Entity CreateCurtain(Engine::Core &core, glm::vec3 position) {
    // Large cloth (3m x 4m, 30x40 grid)
    auto curtain = Physics::Resource::SoftBodyBuilder::CreateCloth(
        core,
        position,
        glm::vec2(3, 4),
        glm::ivec2(30, 40),
        Physics::Component::SoftBodySettings::Cloth()
    );
    
    // Pin top row
    auto &cloth = curtain.GetComponent<Physics::Component::Cloth>(core);
    cloth.PinRow(0);
    
    // Heavier cloth (less wind effect)
    cloth.settings.particleMass = 0.2f;
    cloth.settings.enableWind = false;
    
    Logger::Info("Curtain created");
    return curtain;
}
```

### Example 3: Create Rope Bridge

```cpp
// Create rope bridge between two points
void CreateRopeBridge(Engine::Core &core, glm::vec3 pointA, glm::vec3 pointB) {
    float distance = glm::length(pointB - pointA);
    
    // Create rope (30 segments)
    auto rope = Physics::Resource::SoftBodyBuilder::CreateRope(
        core,
        pointA,
        pointB,
        distance,
        30,
        Physics::Component::SoftBodySettings::Rope()
    );
    
    // Pin both ends
    auto &ropeComp = rope.GetComponent<Physics::Component::Rope>(core);
    ropeComp.PinEnd(true);   // Start
    ropeComp.PinEnd(false);  // End
    
    // Add planks (rigid bodies attached to rope)
    int plankCount = 10;
    for (int i = 1; i < plankCount - 1; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(plankCount);
        glm::vec3 plankPos = glm::mix(pointA, pointB, t);
        
        auto plank = CreatePlank(core, plankPos, 0.6f, 0.1f, 0.05f);
        
        // Attach plank to rope particle
        int particleIdx = static_cast<int>(t * ropeComp.particles.size());
        ropeComp.AttachParticle(particleIdx, plank);
    }
    
    Logger::Info("Rope bridge created ({} planks)", plankCount);
}
```

### Example 4: Hanging Rope with Weight

```cpp
// Create rope hanging from ceiling with weight at end
Engine::Entity CreateHangingRope(Engine::Core &core, glm::vec3 ceilingPos) {
    // Create rope (1m long, 15 segments)
    auto rope = Physics::Resource::SoftBodyBuilder::CreateRope(
        core,
        ceilingPos,
        ceilingPos + glm::vec3(0, -1, 0),
        1.0f,
        15,
        Physics::Component::SoftBodySettings::Rope()
    );
    
    // Pin top end
    auto &ropeComp = rope.GetComponent<Physics::Component::Rope>(core);
    ropeComp.PinEnd(true);
    
    // Create weight at bottom
    auto weight = core.CreateEntity();
    weight.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(ceilingPos + glm::vec3(0, -1, 0)));
    
    auto rb = Physics::Component::RigidBody::CreateDynamic(5.0f);
    weight.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    Physics::Component::SphereCollider collider(0.1f);
    weight.AddComponent<Physics::Component::SphereCollider>(core, collider);
    
    // Attach weight to rope end
    int lastParticle = static_cast<int>(ropeComp.particles.size()) - 1;
    ropeComp.AttachParticle(lastParticle, weight);
    
    Logger::Info("Hanging rope created with weight");
    return rope;
}
```

### Example 5: Soft Jello Cube

```cpp
// Create soft deformable cube (jello)
Engine::Entity CreateJelloCube(Engine::Core &core, glm::vec3 position) {
    // Soft cube (1m size, 8x8x8 grid)
    auto jello = Physics::Resource::SoftBodyBuilder::CreateSoftCube(
        core,
        position,
        1.0f,
        8,
        Physics::Component::SoftBodySettings::Default()
    );
    
    // Very soft settings
    auto &softBody = jello.GetComponent<Physics::Component::SoftBody>(core);
    softBody.settings.stiffness = 0.3f;
    softBody.settings.damping = 0.5f;
    
    Logger::Info("Jello cube created ({}x{}x{} particles)", 8, 8, 8);
    return jello;
}
```

### Example 6: Cut/Tear Cloth

```cpp
// Cut cloth with sword/blade
void CutCloth(Engine::Core &core, Engine::Entity cloth, glm::vec3 cutPos, float cutRadius) {
    Physics::Resource::SoftBodyBuilder::Cut(core, cloth, cutPos, cutRadius);
    
    Logger::Info("Cloth cut at position ({}, {}, {})", cutPos.x, cutPos.y, cutPos.z);
    
    // Spawn tear effect
    Particles::SpawnClothTear(core, cutPos);
    Audio::PlaySound("sounds/cloth_tear.wav", cutPos);
}

// Example: Sword slash cuts flag
void OnSwordHit(Engine::Core &core, Engine::Entity sword, Engine::Entity flag) {
    auto &swordTransform = sword.GetComponent<Object::Component::Transform>(core);
    
    // Cut at sword tip position
    glm::vec3 swordTip = swordTransform.position + swordTransform.GetForward() * 1.0f;
    CutCloth(core, flag, swordTip, 0.2f);
}
```

### Example 7: Wind Control

```cpp
// Control wind dynamically
void UpdateWind(Engine::Core &core, Engine::Entity cloth, float time) {
    auto &clothComp = cloth.GetComponent<Physics::Component::Cloth>(core);
    
    // Oscillating wind (simulates gusts)
    float windStrength = 10.0f + 5.0f * glm::sin(time * 2.0f);
    glm::vec3 windDir = glm::vec3(
        glm::cos(time * 0.5f),
        0.0f,
        glm::sin(time * 0.5f)
    );
    
    Physics::Resource::SoftBodyBuilder::ApplyWind(
        core, cloth, windDir, windStrength
    );
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid grid size**
   ```cpp
   if (gridSize.x < 2 || gridSize.y < 2) {
       Logger::Error("SoftBodyBuilder: Grid size too small (min 2x2)");
       return;
   }
   ```

2. **Zero segments (rope)**
   ```cpp
   if (segments < 2) {
       Logger::Error("SoftBodyBuilder: Rope needs at least 2 segments");
       return;
   }
   ```

3. **Invalid particle index**
   ```cpp
   if (index < 0 || index >= particles.size()) {
       Logger::Error("SoftBody: Invalid particle index {}", index);
       return;
   }
   ```

4. **Zero mass particles**
   ```cpp
   if (particleMass <= 0.0f) {
       Logger::Warning("SoftBody: Particle mass must be positive, using 1.0");
       particleMass = 1.0f;
   }
   ```

## 💡 Design Considerations

### Mass-Spring System

```
Particle: F = ma
Spring: F = -k * (distance - restLength) * direction
Damping: F = -c * velocity
```

### Constraint Solving (Iterative)

```cpp
for (int iter = 0; iter < solverIterations; ++iter) {
    for (auto &constraint : constraints) {
        // Calculate constraint error
        // Apply position correction
    }
}
```

### Performance Notes

- Each particle is simulated per frame
- Constraint solving is O(n * iterations)
- Recommended: max 1000 particles per soft body
- Use spatial hashing for collision detection

## 📊 Success Criteria

- [ ] SoftBody component implemented
- [ ] Cloth component implemented
- [ ] Rope component implemented
- [ ] SoftBodyBuilder implemented
- [ ] Cloth generation working
- [ ] Rope generation working
- [ ] Constraint solving working
- [ ] Wind force working
- [ ] Pinning working
- [ ] Cutting/tearing working
- [ ] Attachments working
- [ ] SoftBodySystem implemented
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 1000 particles)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Soft Body: https://jrouwe.github.io/JoltPhysics/class_soft_body_shared_settings.html
- Cloth Simulation: http://graphics.stanford.edu/~mdfisher/cloth.html
- Unity Cloth: https://docs.unity3d.com/Manual/class-Cloth.html
- Position Based Dynamics: https://matthias-research.github.io/pages/publications/posBasedDyn.pdf

## 🚀 Follow-up Issues

- #024: Performance Tuning (optimize constraint solving)
- #020: Debug Visualization (show particles, constraints)
- #023: GPU Cloth Simulation (compute shaders)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.7`, `priority:medium`, `soft-body`, `cloth`, `rope`  
**Milestone:** v0.7 - Advanced Physics
