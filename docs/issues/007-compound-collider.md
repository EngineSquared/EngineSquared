# Issue #007: Implement CompoundCollider

**Milestone:** v0.4 - Collider Expansion  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 4-5 days  
**Dependencies:** #005 (Primitives), #006 (MeshCollider)  
**Related Issues:** #008 (HeightFieldCollider)

---

## 📋 Description

Implement `CompoundCollider` component to combine multiple collision shapes into a single rigid body. Essential for complex objects that can't be represented by a single primitive (vehicles, furniture, ragdolls).

CompoundColliders allow building complex collision shapes from simpler parts, improving both performance and accuracy compared to using a single complex mesh.

## 🎯 Goals

- ✅ Implement CompoundCollider component
- ✅ Support all existing collider types (Box, Sphere, Capsule, Mesh)
- ✅ Support per-child transforms (position, rotation, scale)
- ✅ Support dynamic and static bodies
- ✅ Automatic compound shape creation from children
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── CompoundCollider.hpp     # New file
│   └── CompoundCollider.cpp     # New file
├── system/
│   └── RigidBodySystem.cpp      # Update shape dispatcher
└── tests/
    └── CompoundColliderTest.cpp # New file
```

### CompoundCollider Component

```cpp
/**************************************************************************
 * @file CompoundCollider.hpp
 * @brief Compound collision shape component (multiple sub-shapes)
 **************************************************************************/

#pragma once

#include <vector>
#include <variant>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "BoxCollider.hpp"
#include "SphereCollider.hpp"
#include "CapsuleCollider.hpp"
#include "MeshCollider.hpp"

namespace Physics::Component {

/**
 * @brief Compound collision shape (multiple sub-shapes)
 * 
 * Combines multiple collision shapes into a single rigid body.
 * Each child shape has its own position, rotation, and scale relative
 * to the parent body.
 * 
 * Common uses:
 * - Vehicles (chassis + wheels)
 * - Furniture (table = top + legs)
 * - Complex props (barrel = cylinder + spheres)
 * - Ragdolls (body parts)
 * - Doors with handles
 * 
 * **Benefits:**
 * - More accurate than single primitive
 * - Faster than complex mesh
 * - Dynamic compatible
 * - Flexible composition
 * 
 * @note Compound shapes have slightly higher overhead than primitives
 * @note Prefer primitives when a single shape suffices
 */
struct CompoundCollider {
    /// Child shape variant (any collider type)
    using ChildShape = std::variant<
        BoxCollider,
        SphereCollider,
        CapsuleCollider,
        MeshCollider
    >;

    /// Child shape with transform
    struct Child {
        ChildShape shape;                     ///< Collision shape
        glm::vec3 position = glm::vec3(0.0f); ///< Local position offset
        glm::quat rotation = glm::quat(1, 0, 0, 0); ///< Local rotation
        glm::vec3 scale = glm::vec3(1.0f);    ///< Local scale (optional)

        /**
         * @brief Construct child shape
         * @param s Shape (Box, Sphere, Capsule, or Mesh)
         * @param pos Local position offset
         * @param rot Local rotation
         * @param scl Local scale
         */
        Child(ChildShape s, 
              glm::vec3 pos = glm::vec3(0.0f),
              glm::quat rot = glm::quat(1, 0, 0, 0),
              glm::vec3 scl = glm::vec3(1.0f))
            : shape(std::move(s)), position(pos), rotation(rot), scale(scl) {}
    };

    std::vector<Child> children;  ///< Child shapes with transforms

    /**
     * @brief Construct empty compound collider
     */
    CompoundCollider() = default;

    /**
     * @brief Construct with initial children
     * @param childs Initial child shapes
     */
    explicit CompoundCollider(std::vector<Child> childs)
        : children(std::move(childs)) {}

    /**
     * @brief Factory method for compound collider
     * @return Empty CompoundCollider
     * 
     * @example
     * @code
     * auto compound = Physics::Component::CompoundCollider::Create();
     * compound.AddBox(glm::vec3(1, 0.5, 1), glm::vec3(0, 0, 0));
     * compound.AddSphere(0.5f, glm::vec3(0, 1, 0));
     * entity.AddComponent<Physics::Component::CompoundCollider>(core, compound);
     * @endcode
     */
    static CompoundCollider Create() {
        return CompoundCollider();
    }

    /**
     * @brief Add box child
     * @param halfExtents Box half extents
     * @param position Local position
     * @param rotation Local rotation
     */
    void AddBox(const glm::vec3 &halfExtents,
                const glm::vec3 &position = glm::vec3(0.0f),
                const glm::quat &rotation = glm::quat(1, 0, 0, 0)) {
        children.emplace_back(BoxCollider(halfExtents), position, rotation);
    }

    /**
     * @brief Add sphere child
     * @param radius Sphere radius
     * @param position Local position
     */
    void AddSphere(float radius,
                   const glm::vec3 &position = glm::vec3(0.0f)) {
        children.emplace_back(SphereCollider(radius), position, glm::quat(1, 0, 0, 0));
    }

    /**
     * @brief Add capsule child
     * @param height Capsule height
     * @param radius Capsule radius
     * @param position Local position
     * @param rotation Local rotation
     */
    void AddCapsule(float height, float radius,
                    const glm::vec3 &position = glm::vec3(0.0f),
                    const glm::quat &rotation = glm::quat(1, 0, 0, 0)) {
        children.emplace_back(CapsuleCollider(height, radius), position, rotation);
    }

    /**
     * @brief Add mesh child (convex hull)
     * @param mesh Mesh collider
     * @param position Local position
     * @param rotation Local rotation
     */
    void AddMesh(const MeshCollider &mesh,
                 const glm::vec3 &position = glm::vec3(0.0f),
                 const glm::quat &rotation = glm::quat(1, 0, 0, 0)) {
        children.emplace_back(mesh, position, rotation);
    }

    /**
     * @brief Validate compound collider
     * @return true if valid, false otherwise
     */
    bool IsValid() const {
        return !children.empty();
    }

    /**
     * @brief Get child count
     * @return Number of child shapes
     */
    size_t GetChildCount() const {
        return children.size();
    }

    /**
     * @brief Clear all children
     */
    void Clear() {
        children.clear();
    }

    /**
     * @brief Calculate total center of mass offset
     * @return Center of mass in local space
     * 
     * @note This is an approximation assuming uniform density
     */
    glm::vec3 CalculateCenterOfMass() const;

    /**
     * @brief Estimate memory usage
     * @return Approximate memory in bytes
     */
    size_t EstimateMemoryUsage() const;
};

} // namespace Physics::Component
```

### Update Shape Dispatcher

```cpp
JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
    // Priority: Compound > Box > Sphere > Capsule > Mesh > Default
    
    // 1. Try CompoundCollider (NEW - highest priority for explicit composition)
    if (registry.all_of<Component::CompoundCollider>(entity)) {
        auto &compound = registry.get<Component::CompoundCollider>(entity);
        
        if (!compound.IsValid()) {
            Logger::Error("CompoundCollider: No child shapes");
            return CreateDefaultShape(registry, entity);
        }
        
        // Create Jolt compound shape
        JPH::StaticCompoundShapeSettings compoundSettings;
        
        for (const auto &child : compound.children) {
            // Convert transform
            JPH::Vec3 pos = Resource::ToJoltVec3(child.position);
            JPH::Quat rot = Resource::ToJoltQuat(child.rotation);
            
            // Create child shape based on variant type
            JPH::ShapeSettings *childShape = nullptr;
            std::visit([&](auto &&shape) {
                using T = std::decay_t<decltype(shape)>;
                if constexpr (std::is_same_v<T, Component::BoxCollider>) {
                    childShape = new JPH::BoxShapeSettings(Resource::ToJoltVec3(shape.halfExtents));
                } else if constexpr (std::is_same_v<T, Component::SphereCollider>) {
                    childShape = new JPH::SphereShapeSettings(shape.radius);
                } else if constexpr (std::is_same_v<T, Component::CapsuleCollider>) {
                    childShape = new JPH::CapsuleShapeSettings(shape.height * 0.5f, shape.radius);
                } else if constexpr (std::is_same_v<T, Component::MeshCollider>) {
                    // Convert mesh to convex hull
                    std::vector<JPH::Float3> vertices;
                    for (const auto &v : shape.vertices) {
                        vertices.push_back(Resource::ToJoltFloat3(v));
                    }
                    childShape = new JPH::ConvexHullShapeSettings(vertices, shape.maxConvexRadius);
                }
            }, child.shape);
            
            // Add to compound
            if (childShape) {
                compoundSettings.AddShape(pos, rot, childShape);
            }
        }
        
        return &compoundSettings;
    }
    
    // ... existing colliders ...
}
```

## 📝 Implementation Tasks

### Phase 1: Create CompoundCollider Component (3-4 hours)

- [ ] Create `component/CompoundCollider.hpp`
- [ ] Implement CompoundCollider struct
- [ ] Implement Child struct with transform
- [ ] Add ChildShape variant (Box, Sphere, Capsule, Mesh)
- [ ] Add helper methods (AddBox, AddSphere, AddCapsule, AddMesh)
- [ ] Add validation methods
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Update RigidBodySystem (4-5 hours)

- [ ] Update shape dispatcher in `system/RigidBodySystem.cpp`
- [ ] Add CompoundCollider case (highest priority)
- [ ] Convert child transforms to Jolt
- [ ] Create JPH::StaticCompoundShapeSettings
- [ ] Use std::visit to handle variant types
- [ ] Create appropriate child shapes
- [ ] Add shapes to compound
- [ ] Handle empty compound error

### Phase 3: Center of Mass Calculation (2-3 hours)

- [ ] Implement `CalculateCenterOfMass()`
- [ ] Calculate volume/mass for each child shape
- [ ] Weight positions by mass
- [ ] Return weighted average
- [ ] Add unit tests for COM calculation

### Phase 4: Unit Tests (5-6 hours)

- [ ] Create `tests/CompoundColliderTest.cpp`
- [ ] Test empty compound (should fail)
- [ ] Test single child (should work)
- [ ] Test multiple children (box + sphere)
- [ ] Test child transforms (position, rotation)
- [ ] Test all child shape types
- [ ] Test compound collision with other bodies
- [ ] Test COM calculation
- [ ] Test memory estimation

### Phase 5: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Table" example
  - Box top + 4 capsule legs
  - Static or dynamic
- [ ] Add "Dumbbell" example
  - Capsule bar + 2 sphere weights
  - Dynamic with rotation
- [ ] Add "Vehicle Chassis" example
  - Box body + 4 sphere wheels (simplified)
  - Show compound as vehicle base
- [ ] Add "Complex Door" example
  - Box door + sphere handle

### Phase 6: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Compound Colliders" section
  - Explain when to use compounds
  - Performance considerations
- [ ] Add examples for common patterns
  - Furniture
  - Vehicles
  - Props

## 🧪 Testing Checklist

### Unit Tests

- [ ] Empty compound is rejected
- [ ] Single child compound works
- [ ] Multiple children compound works
- [ ] All child shape types supported
- [ ] Child transforms applied correctly
- [ ] Compound collides with primitives
- [ ] Compound collides with other compounds
- [ ] COM calculation is accurate
- [ ] Memory estimation is reasonable

### Integration Tests

- [ ] Table (box + legs) is stable
- [ ] Dumbbell rolls correctly
- [ ] Vehicle chassis collides properly
- [ ] Complex shapes collide accurately

### Performance Tests

- [ ] 2-child compound: < 0.5ms overhead vs single primitive
- [ ] 10-child compound: < 2ms overhead
- [ ] 100 compound bodies: < 10ms per frame

### Edge Cases

- [ ] Overlapping children (should warn)
- [ ] Very large number of children (100+)
- [ ] Children with extreme transforms
- [ ] Compound with mesh children

## 📚 Code Examples

### Example 1: Table (Box Top + Capsule Legs)

```cpp
auto table = core.CreateEntity();

// Transform
table.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

// Physics: static body with compound collider
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
table.AddComponent<Physics::Component::RigidBody>(core, rb);

// Compound collider: table top + 4 legs
Physics::Component::CompoundCollider compound;

// Table top (centered at origin)
compound.AddBox(glm::vec3(1.5f, 0.05f, 1.0f), glm::vec3(0, 0.5f, 0));

// Legs (capsules at corners)
float legHeight = 0.5f;
float legRadius = 0.05f;
float legOffset = 1.3f;
compound.AddCapsule(legHeight, legRadius, glm::vec3(-legOffset, 0, -0.8f));
compound.AddCapsule(legHeight, legRadius, glm::vec3(legOffset, 0, -0.8f));
compound.AddCapsule(legHeight, legRadius, glm::vec3(-legOffset, 0, 0.8f));
compound.AddCapsule(legHeight, legRadius, glm::vec3(legOffset, 0, 0.8f));

table.AddComponent<Physics::Component::CompoundCollider>(core, compound);
```

### Example 2: Dumbbell (Capsule + Spheres)

```cpp
auto dumbbell = core.CreateEntity();

// Transform
dumbbell.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 2, 0)));

// Physics: dynamic body
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(10.0f);
dumbbell.AddComponent<Physics::Component::RigidBody>(core, rb);

// Compound: bar + 2 weights
Physics::Component::CompoundCollider compound;

// Central bar (capsule)
compound.AddCapsule(1.0f, 0.05f, glm::vec3(0, 0, 0),
                    glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)));

// Weights (spheres at ends)
compound.AddSphere(0.3f, glm::vec3(-0.6f, 0, 0));
compound.AddSphere(0.3f, glm::vec3(0.6f, 0, 0));

dumbbell.AddComponent<Physics::Component::CompoundCollider>(core, compound);
```

### Example 3: Vehicle Chassis (Simplified)

```cpp
auto vehicle = core.CreateEntity();

// Transform
vehicle.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 1, 0)));

// Physics: dynamic body
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(1000.0f);
vehicle.AddComponent<Physics::Component::RigidBody>(core, rb);

// Compound: body + wheels (simplified representation)
Physics::Component::CompoundCollider compound;

// Chassis (box)
compound.AddBox(glm::vec3(1.0f, 0.3f, 2.0f), glm::vec3(0, 0.5f, 0));

// Wheels (spheres for simplicity - real vehicle would use constraints)
float wheelRadius = 0.3f;
float wheelOffset = 0.8f;
compound.AddSphere(wheelRadius, glm::vec3(-0.8f, 0, -wheelOffset));
compound.AddSphere(wheelRadius, glm::vec3(0.8f, 0, -wheelOffset));
compound.AddSphere(wheelRadius, glm::vec3(-0.8f, 0, wheelOffset));
compound.AddSphere(wheelRadius, glm::vec3(0.8f, 0, wheelOffset));

vehicle.AddComponent<Physics::Component::CompoundCollider>(core, compound);
```

### Example 4: Barrel (Cylinder Approximation)

```cpp
// Approximate cylinder with stacked boxes (better than single capsule)
auto barrel = core.CreateEntity();

Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(20.0f);
barrel.AddComponent<Physics::Component::RigidBody>(core, rb);

Physics::Component::CompoundCollider compound;

// Stack 5 boxes to approximate cylinder
float height = 1.0f;
float radius = 0.3f;
int segments = 5;
for (int i = 0; i < segments; ++i) {
    float y = (i - segments / 2.0f) * (height / segments);
    compound.AddBox(glm::vec3(radius, height / (segments * 2), radius),
                    glm::vec3(0, y, 0));
}

barrel.AddComponent<Physics::Component::CompoundCollider>(core, compound);
```

### Example 5: Complex Prop from Multiple Meshes

```cpp
// Statue with multiple mesh parts
Object::OBJLoader bodyLoader("assets/statue_body.obj");
Object::OBJLoader baseLoader("assets/statue_base.obj");

auto statue = core.CreateEntity();

Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(100.0f);
statue.AddComponent<Physics::Component::RigidBody>(core, rb);

Physics::Component::CompoundCollider compound;

// Body (convex hull from mesh)
auto bodyMesh = Physics::Component::MeshCollider::CreateFromMesh(
    bodyLoader.GetMesh(), true  // Convex hull
);
compound.AddMesh(bodyMesh, glm::vec3(0, 1, 0));

// Base (convex hull from mesh)
auto baseMesh = Physics::Component::MeshCollider::CreateFromMesh(
    baseLoader.GetMesh(), true
);
compound.AddMesh(baseMesh, glm::vec3(0, 0, 0));

statue.AddComponent<Physics::Component::CompoundCollider>(core, compound);
```

## 🐛 Error Handling

### Cases to Handle

1. **Empty compound**
   ```cpp
   if (!compound.IsValid()) {
       Logger::Error("CompoundCollider: No child shapes defined");
       return nullptr;  // Fallback to DefaultCollider
   }
   ```

2. **Invalid child shape**
   ```cpp
   std::visit([&](auto &&shape) {
       if (!shape.IsValid()) {
           Logger::Warn("CompoundCollider: Invalid child shape (skipping)");
           return;
       }
       // ... create shape ...
   }, child.shape);
   ```

3. **Too many children**
   ```cpp
   if (compound.GetChildCount() > 100) {
       Logger::Warn("CompoundCollider: Large number of children ({}) may impact performance",
                    compound.GetChildCount());
   }
   ```

4. **Overlapping children**
   ```cpp
   // Check for overlaps (optional, expensive)
   for (size_t i = 0; i < compound.children.size(); ++i) {
       for (size_t j = i + 1; j < compound.children.size(); ++j) {
           if (ShapesOverlap(compound.children[i], compound.children[j])) {
               Logger::Warn("CompoundCollider: Overlapping children detected");
           }
       }
   }
   ```

## 💡 Design Considerations

### StaticCompound vs MutableCompound

Jolt provides two compound types:
- **StaticCompound**: Immutable after creation (faster, most cases)
- **MutableCompound**: Can add/remove shapes at runtime (slower, rare)

**EngineSquared uses StaticCompound** for simplicity and performance.
For dynamic shape changes, destroy and recreate the body.

### Child Shape Priority

When using different collider types:
1. **Prefer primitives** (Box, Sphere, Capsule) over Mesh
2. **Use Mesh** only when primitive approximation insufficient
3. **Keep child count low** (< 10 shapes for optimal performance)

### Performance vs Accuracy Trade-off

```cpp
// ❌ TOO COMPLEX: 50 boxes to approximate complex shape
// Result: Slow collision detection
for (int i = 0; i < 50; ++i) {
    compound.AddBox(/* ... */);
}

// ✅ BETTER: Simplified compound (3-5 shapes)
compound.AddBox(/* main body */);
compound.AddSphere(/* rounded corner */);
compound.AddCapsule(/* arm */);

// ✅ BEST: Single mesh collider (convex hull)
auto mesh = MeshCollider::CreateFromMesh(complexMesh, true);
entity.AddComponent<MeshCollider>(core, mesh);
```

### Center of Mass

Jolt automatically calculates COM from compound geometry.
For custom COM, use Jolt's `mCenterOfMassOverride` (future enhancement).

## 📊 Success Criteria

- [ ] CompoundCollider component implemented
- [ ] All child shape types supported
- [ ] Child transforms working correctly
- [ ] RigidBodySystem handles compounds
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 2ms for 10 children)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt CompoundShape: https://jrouwe.github.io/JoltPhysics/class_static_compound_shape.html
- Unity Compound Collider: (No direct equivalent - use multiple colliders)
- Godot CompoundShape: https://docs.godotengine.org/en/stable/classes/class_compoundshape3d.html (Godot 4)
- Bullet Compound Shape: https://pybullet.org/Bullet/BulletFull/classbtCompoundShape.html

## 🚀 Follow-up Issues

- #008: HeightFieldCollider (terrain)
- #015: MutableCompound (runtime shape changes)
- #016: Automatic compound decomposition (split complex mesh)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.4`, `priority:high`  
**Milestone:** v0.4 - Collider Expansion
