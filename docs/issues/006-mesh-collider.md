# Issue #006: Implement MeshCollider

**Milestone:** v0.4 - Collider Expansion  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 5-7 days  
**Dependencies:** #005 (SphereCollider/CapsuleCollider)  
**Related Issues:** #007 (CompoundCollider)

---

## 📋 Description

Implement `MeshCollider` component for arbitrary 3D geometry collision. Supports both **convex hulls** (fast, dynamic compatible) and **triangle meshes** (accurate, static only).

MeshColliders allow using custom 3D models as collision shapes, essential for complex static geometry (buildings, props, terrain features) and simplified dynamic objects.

## 🎯 Goals

- ✅ Implement MeshCollider component (vertices + indices)
- ✅ Support convex hull generation (dynamic objects)
- ✅ Support triangle mesh (static objects only)
- ✅ Integration with Object::Component::Mesh
- ✅ Automatic convex decomposition (optional)
- ✅ Mesh validation and error handling
- ✅ Performance optimization (mesh simplification)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── MeshCollider.hpp         # New file
│   └── MeshCollider.cpp         # New file
├── resource/
│   ├── MeshProcessor.hpp        # Mesh processing utilities
│   └── MeshProcessor.cpp        # Convex hull, simplification
├── system/
│   └── RigidBodySystem.cpp      # Update shape dispatcher
└── tests/
    └── MeshColliderTest.cpp     # New file
```

### MeshCollider Component

```cpp
/**************************************************************************
 * @file MeshCollider.hpp
 * @brief Mesh collision shape component
 **************************************************************************/

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Mesh collision shape for arbitrary 3D geometry
 * 
 * Supports two modes:
 * 1. **Convex Hull**: Fast, suitable for dynamic bodies (simplified convex shape)
 * 2. **Triangle Mesh**: Accurate, static bodies only (exact geometry)
 * 
 * **Convex Hull** should be preferred when possible (much faster).
 * **Triangle Mesh** is for static world geometry only.
 * 
 * Common uses:
 * - Static: Buildings, props, terrain features (triangle mesh)
 * - Dynamic: Custom shapes simplified to convex hull
 * - Triggers: Complex trigger volumes
 * 
 * @warning Triangle meshes are STATIC ONLY (cannot be dynamic/kinematic)
 * @warning Large triangle meshes can be slow - use primitives when possible
 */
struct MeshCollider {
    /// Collision mesh type
    enum class MeshType {
        ConvexHull,     ///< Convex hull (fast, dynamic compatible)
        TriangleMesh    ///< Triangle mesh (accurate, static only)
    };

    std::vector<glm::vec3> vertices;  ///< Mesh vertices
    std::vector<uint32_t> indices;    ///< Triangle indices (triplets)
    MeshType meshType = MeshType::ConvexHull;  ///< Collision type

    // Optional: Mesh simplification
    float maxConvexRadius = 0.05f;    ///< Max distance for convex hull (0 = exact)
    bool autoSimplify = true;         ///< Automatically simplify complex meshes

    /**
     * @brief Construct mesh collider
     * @param verts Vertex positions
     * @param inds Triangle indices
     * @param type Mesh type (convex hull or triangle mesh)
     */
    MeshCollider(const std::vector<glm::vec3> &verts = {},
                 const std::vector<uint32_t> &inds = {},
                 MeshType type = MeshType::ConvexHull)
        : vertices(verts), indices(inds), meshType(type) {}

    /**
     * @brief Create convex hull mesh collider
     * @param verts Vertex positions
     * @param inds Triangle indices
     * @return MeshCollider configured for convex hull
     * 
     * @example
     * @code
     * auto mesh = entity.GetComponent<Object::Component::Mesh>(core);
     * auto collider = Physics::Component::MeshCollider::CreateConvexHull(
     *     mesh.vertices, mesh.indices
     * );
     * entity.AddComponent<Physics::Component::MeshCollider>(core, collider);
     * @endcode
     */
    static MeshCollider CreateConvexHull(const std::vector<glm::vec3> &verts,
                                         const std::vector<uint32_t> &inds) {
        return MeshCollider(verts, inds, MeshType::ConvexHull);
    }

    /**
     * @brief Create triangle mesh collider (STATIC ONLY)
     * @param verts Vertex positions
     * @param inds Triangle indices
     * @return MeshCollider configured for triangle mesh
     * 
     * @warning Only use with static bodies
     * 
     * @example
     * @code
     * auto mesh = entity.GetComponent<Object::Component::Mesh>(core);
     * auto collider = Physics::Component::MeshCollider::CreateTriangleMesh(
     *     mesh.vertices, mesh.indices
     * );
     * entity.AddComponent<Physics::Component::MeshCollider>(core, collider);
     * @endcode
     */
    static MeshCollider CreateTriangleMesh(const std::vector<glm::vec3> &verts,
                                           const std::vector<uint32_t> &inds) {
        return MeshCollider(verts, inds, MeshType::TriangleMesh);
    }

    /**
     * @brief Create from Object::Component::Mesh
     * @param mesh Source mesh component
     * @param convex Use convex hull (true) or triangle mesh (false)
     * @return MeshCollider from mesh data
     */
    static MeshCollider CreateFromMesh(const Object::Component::Mesh &mesh, 
                                       bool convex = true);

    /**
     * @brief Validate mesh collider properties
     * @return true if valid, false otherwise
     */
    bool IsValid() const {
        return !vertices.empty() && 
               indices.size() >= 3 && 
               indices.size() % 3 == 0;
    }

    /**
     * @brief Get triangle count
     * @return Number of triangles
     */
    size_t GetTriangleCount() const {
        return indices.size() / 3;
    }

    /**
     * @brief Get vertex count
     * @return Number of vertices
     */
    size_t GetVertexCount() const {
        return vertices.size();
    }

    /**
     * @brief Check if mesh is suitable for convex hull
     * @return true if mesh is already convex or can be simplified
     */
    bool IsConvexCompatible() const;

    /**
     * @brief Estimate memory usage
     * @return Approximate memory in bytes
     */
    size_t EstimateMemoryUsage() const {
        return vertices.size() * sizeof(glm::vec3) + 
               indices.size() * sizeof(uint32_t);
    }
};

} // namespace Physics::Component
```

### MeshProcessor Resource

```cpp
namespace Physics::Resource {

/**
 * @brief Mesh processing utilities
 */
class MeshProcessor {
public:
    /**
     * @brief Simplify mesh for collision (reduce vertex count)
     * @param vertices Input vertices
     * @param indices Input indices
     * @param targetReduction Reduction factor (0.5 = 50% fewer vertices)
     * @return Simplified mesh data
     */
    static std::pair<std::vector<glm::vec3>, std::vector<uint32_t>>
    SimplifyMesh(const std::vector<glm::vec3> &vertices,
                 const std::vector<uint32_t> &indices,
                 float targetReduction = 0.5f);

    /**
     * @brief Validate mesh topology
     * @param vertices Mesh vertices
     * @param indices Mesh indices
     * @return true if valid, false otherwise
     */
    static bool ValidateMesh(const std::vector<glm::vec3> &vertices,
                             const std::vector<uint32_t> &indices);

    /**
     * @brief Calculate mesh bounds
     * @param vertices Mesh vertices
     * @return Min and max bounds
     */
    static std::pair<glm::vec3, glm::vec3>
    CalculateBounds(const std::vector<glm::vec3> &vertices);

    /**
     * @brief Check if mesh is convex
     * @param vertices Mesh vertices
     * @param indices Mesh indices
     * @return true if convex, false otherwise
     */
    static bool IsConvex(const std::vector<glm::vec3> &vertices,
                         const std::vector<uint32_t> &indices);

    /**
     * @brief Decompose concave mesh into multiple convex hulls (V-HACD)
     * @param vertices Input vertices
     * @param indices Input indices
     * @param maxHulls Maximum number of hulls
     * @return Vector of convex hull meshes
     * 
     * @note This is expensive - cache results!
     */
    static std::vector<std::pair<std::vector<glm::vec3>, std::vector<uint32_t>>>
    DecomposeConvex(const std::vector<glm::vec3> &vertices,
                    const std::vector<uint32_t> &indices,
                    uint32_t maxHulls = 32);
};

} // namespace Physics::Resource
```

### Update Shape Dispatcher

```cpp
JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
    // Priority: Box > Sphere > Capsule > Mesh > Default
    
    // ... existing colliders ...
    
    // 4. Try MeshCollider (NEW)
    if (registry.all_of<Component::MeshCollider>(entity)) {
        auto &mesh = registry.get<Component::MeshCollider>(entity);
        
        if (!mesh.IsValid()) {
            Logger::Error("MeshCollider: Invalid mesh data");
            return CreateDefaultShape(registry, entity);
        }
        
        // Convert vertices to Jolt format
        std::vector<JPH::Float3> joltVertices;
        joltVertices.reserve(mesh.vertices.size());
        for (const auto &v : mesh.vertices) {
            joltVertices.push_back(Resource::ToJoltFloat3(v));
        }
        
        // Create appropriate shape
        if (mesh.meshType == Component::MeshCollider::MeshType::ConvexHull) {
            return new JPH::ConvexHullShapeSettings(joltVertices, mesh.maxConvexRadius);
        } else {
            // Triangle mesh (static only)
            JPH::TriangleList triangles;
            for (size_t i = 0; i < mesh.indices.size(); i += 3) {
                JPH::Triangle tri;
                tri.mV[0] = mesh.indices[i];
                tri.mV[1] = mesh.indices[i + 1];
                tri.mV[2] = mesh.indices[i + 2];
                triangles.push_back(tri);
            }
            return new JPH::MeshShapeSettings(joltVertices, triangles);
        }
    }
    
    // 5. Fallback to DefaultCollider
    return CreateDefaultShape(registry, entity);
}
```

## 📝 Implementation Tasks

### Phase 1: Create MeshCollider Component (3-4 hours)

- [ ] Create `component/MeshCollider.hpp`
- [ ] Implement MeshCollider struct with vertices/indices
- [ ] Add MeshType enum (ConvexHull, TriangleMesh)
- [ ] Add factory methods (CreateConvexHull, CreateTriangleMesh, CreateFromMesh)
- [ ] Add validation methods
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create MeshProcessor Resource (4-5 hours)

- [ ] Create `resource/MeshProcessor.hpp`
- [ ] Implement mesh validation
- [ ] Implement bounds calculation
- [ ] Implement convexity test
- [ ] Implement mesh simplification (optional)
- [ ] Implement convex decomposition (V-HACD integration, optional)
- [ ] Add error handling

### Phase 3: Update RigidBodySystem (3-4 hours)

- [ ] Update shape dispatcher in `system/RigidBodySystem.cpp`
- [ ] Add MeshCollider case
- [ ] Convert vertices to Jolt Float3
- [ ] Create ConvexHullShapeSettings for convex mode
- [ ] Create MeshShapeSettings for triangle mode
- [ ] Validate triangle mesh is on static body only
- [ ] Handle large mesh warnings

### Phase 4: Integration with Object Plugin (2-3 hours)

- [ ] Implement `CreateFromMesh()` helper
- [ ] Extract vertices from Object::Component::Mesh
- [ ] Handle vertex format conversion
- [ ] Test with OBJ loader

### Phase 5: Unit Tests (5-6 hours)

- [ ] Create `tests/MeshColliderTest.cpp`
- [ ] Test convex hull creation
- [ ] Test triangle mesh creation
- [ ] Test mesh validation
- [ ] Test invalid meshes (empty, bad indices)
- [ ] Test triangle mesh on dynamic body (should fail)
- [ ] Test mesh simplification
- [ ] Test large mesh performance

### Phase 6: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Static Prop" example (triangle mesh)
  - Load OBJ model
  - Create MeshCollider from mesh
  - Static body
- [ ] Add "Dynamic Custom Shape" example (convex hull)
  - Custom mesh (rock, barrel)
  - Convex hull collider
  - Dynamic body
- [ ] Add "Complex Static Geometry" example
  - Building with multiple floors
  - Triangle mesh
- [ ] Add performance comparison (primitives vs mesh)

### Phase 7: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Mesh Colliders" section
  - Explain convex hull vs triangle mesh
  - Performance guidelines
  - When to use primitives vs meshes
- [ ] Create `docs/MeshColliders.md` (detailed guide)
  - Mesh preparation in 3D software
  - Performance best practices
  - Convex decomposition workflow

## 🧪 Testing Checklist

### Unit Tests

- [ ] Convex hull collider creates valid shape
- [ ] Triangle mesh collider creates valid shape
- [ ] Mesh validation rejects invalid data
- [ ] Triangle mesh on dynamic body is rejected
- [ ] Large meshes trigger warnings
- [ ] Mesh simplification reduces vertex count
- [ ] Bounds calculation is correct
- [ ] Memory estimation is accurate

### Integration Tests

- [ ] Static prop with triangle mesh collides correctly
- [ ] Dynamic object with convex hull collides correctly
- [ ] Mesh loaded from OBJ works
- [ ] Complex mesh (100+ triangles) performs acceptably
- [ ] Mesh collider with primitives collision

### Performance Tests

- [ ] 100 triangle mesh: < 10ms creation
- [ ] 1000 triangle mesh: < 100ms creation
- [ ] Convex hull (50 vertices): < 5ms creation
- [ ] Mesh vs primitives collision cost

### Edge Cases

- [ ] Empty mesh
- [ ] Mesh with < 3 vertices
- [ ] Indices out of bounds
- [ ] Non-manifold mesh
- [ ] Degenerate triangles (zero area)
- [ ] Very large mesh (10,000+ triangles)

## 📚 Code Examples

### Example 1: Static Prop (Triangle Mesh)

```cpp
// Load 3D model
Object::OBJLoader loader("assets/models/building.obj");
auto mesh = loader.GetMesh();

// Create entity with visual mesh
auto building = core.CreateEntity();
building.AddComponent<Object::Component::Transform>(core, 
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));
building.AddComponent<Object::Component::Mesh>(core, mesh);

// Add physics: static body with triangle mesh collider
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
building.AddComponent<Physics::Component::RigidBody>(core, rb);

// Triangle mesh for accurate collision (static only)
auto collider = Physics::Component::MeshCollider::CreateFromMesh(mesh, false);  // false = triangle mesh
building.AddComponent<Physics::Component::MeshCollider>(core, collider);
```

### Example 2: Dynamic Custom Shape (Convex Hull)

```cpp
// Load rock model
Object::OBJLoader loader("assets/models/rock.obj");
auto mesh = loader.GetMesh();

// Create dynamic rock
auto rock = core.CreateEntity();
rock.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 10, 0)));
rock.AddComponent<Object::Component::Mesh>(core, mesh);

// Dynamic body with convex hull (simplified collision)
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(50.0f);
rock.AddComponent<Physics::Component::RigidBody>(core, rb);

// Convex hull for dynamic collision (much faster)
auto collider = Physics::Component::MeshCollider::CreateFromMesh(mesh, true);  // true = convex hull
rock.AddComponent<Physics::Component::MeshCollider>(core, collider);
```

### Example 3: Manual Mesh Collider

```cpp
// Create custom pyramid mesh
std::vector<glm::vec3> vertices = {
    glm::vec3(0, 1, 0),     // Top
    glm::vec3(-1, 0, -1),   // Base corners
    glm::vec3(1, 0, -1),
    glm::vec3(1, 0, 1),
    glm::vec3(-1, 0, 1)
};

std::vector<uint32_t> indices = {
    // Base
    0, 1, 2,  0, 2, 3,  0, 3, 4,  0, 4, 1,
    // Bottom
    1, 4, 3,  1, 3, 2
};

auto pyramid = core.CreateEntity();
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateDynamic(10.0f);
pyramid.AddComponent<Physics::Component::RigidBody>(core, rb);

// Convex hull from manual vertices
auto collider = Physics::Component::MeshCollider::CreateConvexHull(vertices, indices);
pyramid.AddComponent<Physics::Component::MeshCollider>(core, collider);
```

### Example 4: Simplified Collision Mesh

```cpp
// Visual mesh: high detail
Object::OBJLoader visualLoader("assets/models/statue_highpoly.obj");
auto visualMesh = visualLoader.GetMesh();

// Collision mesh: low detail (manually created in Blender)
Object::OBJLoader collisionLoader("assets/models/statue_collision.obj");
auto collisionMesh = collisionLoader.GetMesh();

auto statue = core.CreateEntity();
statue.AddComponent<Object::Component::Mesh>(core, visualMesh);  // Visual

Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
statue.AddComponent<Physics::Component::RigidBody>(core, rb);

// Use simplified collision mesh (much faster)
auto collider = Physics::Component::MeshCollider::CreateFromMesh(collisionMesh, false);
statue.AddComponent<Physics::Component::MeshCollider>(core, collider);
```

### Example 5: Performance Comparison

```cpp
// ❌ BAD: Complex mesh as collider
auto complexMesh = loader.GetMesh();  // 5000 triangles
auto badCollider = Physics::Component::MeshCollider::CreateTriangleMesh(
    complexMesh.vertices, complexMesh.indices
);
// Result: Slow collision detection, high memory usage

// ✅ GOOD: Use primitives when possible
auto box = Physics::Component::BoxCollider(glm::vec3(1, 2, 1));
// Result: 100x faster collision, minimal memory

// ✅ ACCEPTABLE: Simplified convex hull for dynamic
auto simplified = Physics::Resource::MeshProcessor::SimplifyMesh(
    complexMesh.vertices, complexMesh.indices, 0.9f  // 90% reduction
);
auto goodCollider = Physics::Component::MeshCollider::CreateConvexHull(
    simplified.first, simplified.second
);
// Result: Fast collision, reasonable approximation
```

## 🐛 Error Handling

### Cases to Handle

1. **Invalid mesh data**
   ```cpp
   if (!mesh.IsValid()) {
       Logger::Error("MeshCollider: Invalid mesh (vertices: {}, indices: {})",
                     mesh.vertices.size(), mesh.indices.size());
       return nullptr;  // Fallback to DefaultCollider
   }
   ```

2. **Triangle mesh on dynamic body**
   ```cpp
   if (mesh.meshType == MeshType::TriangleMesh && 
       rb.motionType != MotionType::Static) {
       Logger::Error("MeshCollider: Triangle meshes are static only (use convex hull for dynamic)");
       return nullptr;
   }
   ```

3. **Large mesh warning**
   ```cpp
   if (mesh.GetTriangleCount() > 1000) {
       Logger::Warn("MeshCollider: Large mesh ({} triangles) may impact performance. "
                    "Consider using simplified collision mesh or primitives.",
                    mesh.GetTriangleCount());
   }
   ```

4. **Degenerate triangles**
   ```cpp
   for (size_t i = 0; i < indices.size(); i += 3) {
       auto v0 = vertices[indices[i]];
       auto v1 = vertices[indices[i+1]];
       auto v2 = vertices[indices[i+2]];
       float area = glm::length(glm::cross(v1 - v0, v2 - v0)) * 0.5f;
       if (area < 1e-6f) {
           Logger::Warn("MeshCollider: Degenerate triangle detected (index {})", i);
       }
   }
   ```

## 💡 Design Considerations

### Convex Hull vs Triangle Mesh

| Feature | Convex Hull | Triangle Mesh |
|---------|-------------|---------------|
| **Dynamic bodies** | ✅ Yes | ❌ No (static only) |
| **Performance** | ✅ Fast | ⚠️ Slower |
| **Accuracy** | ⚠️ Simplified | ✅ Exact |
| **Memory** | ✅ Low | ⚠️ High |
| **Use cases** | Dynamic objects, simplified collision | Static world geometry |

### When to Use Primitives vs Mesh

**Use Primitives (Box/Sphere/Capsule) when:**
- Shape can be approximated (90% of cases)
- Performance is critical
- Dynamic bodies
- Character controllers

**Use Convex Hull when:**
- Custom shape needed
- Shape is naturally convex
- Dynamic body with complex shape

**Use Triangle Mesh when:**
- Static world geometry
- Exact collision required
- Complex concave shapes (buildings)

### Performance Guidelines

**Mesh Complexity Recommendations:**
- **Convex Hull**: < 50 vertices (optimal: 20-30)
- **Triangle Mesh**: < 500 triangles (optimal: 100-200)
- **For larger meshes**: Use multiple primitives or CompoundCollider

**Optimization Workflow:**
1. Try primitives first (Box, Sphere, Capsule)
2. If needed, use low-poly collision mesh (create in 3D software)
3. If dynamic, use convex hull
4. If static and complex, use triangle mesh
5. For very complex shapes, use CompoundCollider with multiple convex hulls

## 📊 Success Criteria

- [ ] MeshCollider component implemented
- [ ] Convex hull mode working
- [ ] Triangle mesh mode working (static only)
- [ ] CreateFromMesh() helper working
- [ ] RigidBodySystem handles mesh colliders
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 10ms for typical meshes)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt ConvexHullShape: https://jrouwe.github.io/JoltPhysics/class_convex_hull_shape.html
- Jolt MeshShape: https://jrouwe.github.io/JoltPhysics/class_mesh_shape.html
- Unity MeshCollider: https://docs.unity3d.com/ScriptReference/MeshCollider.html
- Godot ConcavePolygonShape3D: https://docs.godotengine.org/en/stable/classes/class_concavepolygonshape3d.html
- V-HACD (convex decomposition): https://github.com/kmammou/v-hacd

## 🚀 Follow-up Issues

- #007: CompoundCollider (combine multiple meshes)
- #008: HeightFieldCollider (optimized terrain)
- #009: Mesh simplification tools
- #010: Convex decomposition (automatic V-HACD)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.4`, `priority:high`  
**Milestone:** v0.4 - Collider Expansion
