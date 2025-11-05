# Issue #008: Implement HeightFieldCollider (Terrain)

**Milestone:** v0.4 - Collider Expansion  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 4-5 days  
**Dependencies:** #006 (MeshCollider)  
**Related Issues:** #007 (CompoundCollider)

---

## 📋 Description

Implement `HeightFieldCollider` component for efficient terrain collision using height maps. Essential for large outdoor environments, landscapes, and procedural terrains.

HeightFieldColliders provide significantly better performance than MeshColliders for terrain by exploiting the regular grid structure and height-only data format.

## 🎯 Goals

- ✅ Implement HeightFieldCollider component
- ✅ Support 2D height map data
- ✅ Configurable resolution and scale
- ✅ Memory-efficient storage
- ✅ Fast collision queries
- ✅ Integration with TerrainProcessor resource
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── HeightFieldCollider.hpp     # New file
│   └── HeightFieldCollider.cpp     # New file
├── resource/
│   ├── TerrainProcessor.hpp        # New file
│   └── TerrainProcessor.cpp        # New file
├── system/
│   └── RigidBodySystem.cpp         # Update shape dispatcher
└── tests/
    ├── HeightFieldColliderTest.cpp # New file
    └── TerrainProcessorTest.cpp    # New file
```

### HeightFieldCollider Component

```cpp
/**************************************************************************
 * @file HeightFieldCollider.hpp
 * @brief Height field collision shape component (terrain)
 **************************************************************************/

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Height field collision shape (optimized for terrain)
 * 
 * Represents terrain using a 2D grid of height values.
 * More efficient than MeshCollider for large flat areas.
 * 
 * **Height field structure:**
 * - 2D grid: `heights[x][z]` = Y coordinate
 * - Regular spacing: uniform X/Z resolution
 * - Height-only: no overhangs or caves
 * 
 * **Common uses:**
 * - Outdoor terrain
 * - Procedural landscapes
 * - Racing tracks
 * - Golf courses
 * - Water surfaces
 * 
 * **Benefits:**
 * - Very fast collision (O(1) height lookup)
 * - Memory efficient (1 float per vertex vs 3)
 * - Large terrains (1000x1000+ vertices)
 * - Easy to generate/modify
 * 
 * **Limitations:**
 * - Static only (no dynamic terrain)
 * - No overhangs or caves
 * - Height-only data
 * 
 * @note HeightFields are ALWAYS static (motion type must be Static)
 * @note For dynamic terrain chunks, use MeshCollider instead
 */
struct HeightFieldCollider {
    /// Height data (row-major: heights[z * width + x])
    std::vector<float> heights;

    /// Terrain dimensions
    uint32_t width = 0;   ///< Number of vertices in X direction
    uint32_t depth = 0;   ///< Number of vertices in Z direction

    /// Terrain scale
    glm::vec3 scale = glm::vec3(1.0f);  ///< Scale (x, y, z)
    
    /// Height range
    float minHeight = 0.0f;  ///< Minimum height value
    float maxHeight = 0.0f;  ///< Maximum height value

    /// Block size (Jolt optimization)
    uint32_t blockSize = 8;  ///< Must be power of 2 (2, 4, 8, 16)

    /**
     * @brief Construct height field from data
     * @param w Width (X vertices)
     * @param d Depth (Z vertices)
     * @param heightData Height values (row-major, size = w * d)
     * @param scl Scale factor (x, y, z)
     * 
     * @example
     * @code
     * // 10x10 flat terrain
     * std::vector<float> heights(10 * 10, 0.0f);
     * auto terrain = Physics::Component::HeightFieldCollider(
     *     10, 10, heights, glm::vec3(1.0f, 1.0f, 1.0f)
     * );
     * @endcode
     */
    HeightFieldCollider(uint32_t w, uint32_t d,
                        const std::vector<float> &heightData,
                        const glm::vec3 &scl = glm::vec3(1.0f))
        : heights(heightData), width(w), depth(d), scale(scl) {
        CalculateHeightRange();
    }

    /**
     * @brief Construct empty height field
     */
    HeightFieldCollider() = default;

    /**
     * @brief Factory method for flat terrain
     * @param w Width (vertices)
     * @param d Depth (vertices)
     * @param height Flat height value
     * @param scl Scale factor
     * @return Flat HeightFieldCollider
     * 
     * @example
     * @code
     * // 100x100 flat ground at Y=0
     * auto ground = Physics::Component::HeightFieldCollider::CreateFlat(
     *     100, 100, 0.0f, glm::vec3(1.0f)
     * );
     * @endcode
     */
    static HeightFieldCollider CreateFlat(uint32_t w, uint32_t d,
                                          float height = 0.0f,
                                          const glm::vec3 &scl = glm::vec3(1.0f)) {
        std::vector<float> heights(w * d, height);
        return HeightFieldCollider(w, d, heights, scl);
    }

    /**
     * @brief Factory method from image
     * @param imagePath Path to height map image (grayscale)
     * @param heightScale Height multiplier (0-255 -> 0-heightScale)
     * @param scl Scale factor (x, y, z)
     * @return HeightFieldCollider from image
     * 
     * @note Uses Object::Resource::ImageLoader internally
     * @note Grayscale value maps to height: black=0, white=heightScale
     * 
     * @example
     * @code
     * // Load terrain from 512x512 heightmap
     * auto terrain = Physics::Component::HeightFieldCollider::CreateFromImage(
     *     "assets/terrain_heightmap.png",
     *     50.0f,  // Max height = 50 units
     *     glm::vec3(1.0f, 1.0f, 1.0f)
     * );
     * @endcode
     */
    static HeightFieldCollider CreateFromImage(const std::string &imagePath,
                                               float heightScale = 10.0f,
                                               const glm::vec3 &scl = glm::vec3(1.0f));

    /**
     * @brief Get height at grid position
     * @param x X index [0, width-1]
     * @param z Z index [0, depth-1]
     * @return Height value
     */
    float GetHeight(uint32_t x, uint32_t z) const {
        if (x >= width || z >= depth) return 0.0f;
        return heights[z * width + x];
    }

    /**
     * @brief Set height at grid position
     * @param x X index [0, width-1]
     * @param z Z index [0, depth-1]
     * @param height New height value
     * 
     * @note Requires recreating Jolt shape after modification
     */
    void SetHeight(uint32_t x, uint32_t z, float height) {
        if (x >= width || z >= depth) return;
        heights[z * width + x] = height;
        CalculateHeightRange();
    }

    /**
     * @brief Interpolate height at world position
     * @param worldX World X position
     * @param worldZ World Z position
     * @return Interpolated height
     * 
     * Uses bilinear interpolation for smooth height lookup.
     */
    float GetHeightAt(float worldX, float worldZ) const;

    /**
     * @brief Validate height field
     * @return true if valid, false otherwise
     */
    bool IsValid() const {
        return width > 0 && depth > 0 && heights.size() == width * depth;
    }

    /**
     * @brief Calculate min/max height range
     */
    void CalculateHeightRange() {
        if (heights.empty()) {
            minHeight = maxHeight = 0.0f;
            return;
        }
        
        minHeight = heights[0];
        maxHeight = heights[0];
        for (float h : heights) {
            minHeight = std::min(minHeight, h);
            maxHeight = std::max(maxHeight, h);
        }
    }

    /**
     * @brief Estimate memory usage
     * @return Approximate memory in bytes
     */
    size_t EstimateMemoryUsage() const {
        return heights.size() * sizeof(float);
    }

    /**
     * @brief Get terrain bounds in world space
     * @return AABB (min, max)
     */
    std::pair<glm::vec3, glm::vec3> GetBounds() const {
        glm::vec3 min(-0.5f * width * scale.x, minHeight * scale.y, -0.5f * depth * scale.z);
        glm::vec3 max(0.5f * width * scale.x, maxHeight * scale.y, 0.5f * depth * scale.z);
        return {min, max};
    }
};

} // namespace Physics::Component
```

### TerrainProcessor Resource

```cpp
/**************************************************************************
 * @file TerrainProcessor.hpp
 * @brief Height field processing utilities (terrain generation/manipulation)
 **************************************************************************/

#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Physics::Component {
    struct HeightFieldCollider;
}

namespace Physics::Resource {

/**
 * @brief Height field processing utilities
 * 
 * Provides functions for generating, smoothing, and manipulating
 * height field terrain data.
 * 
 * **Responsibilities:**
 * - Procedural terrain generation (Perlin noise, etc.)
 * - Height map smoothing/filtering
 * - Terrain modification (raise, lower, flatten)
 * - Normal calculation
 */
class TerrainProcessor {
public:
    /**
     * @brief Generate Perlin noise terrain
     * @param width Terrain width (vertices)
     * @param depth Terrain depth (vertices)
     * @param scale Noise scale (higher = smoother)
     * @param amplitude Height multiplier
     * @param octaves Number of noise layers
     * @param persistence Amplitude decrease per octave
     * @return Height values (row-major)
     * 
     * @example
     * @code
     * // Generate 256x256 hilly terrain
     * auto heights = Physics::Resource::TerrainProcessor::GeneratePerlinTerrain(
     *     256, 256, 50.0f, 20.0f, 4, 0.5f
     * );
     * auto terrain = Physics::Component::HeightFieldCollider(256, 256, heights);
     * @endcode
     */
    static std::vector<float> GeneratePerlinTerrain(
        uint32_t width, uint32_t depth,
        float scale = 50.0f,
        float amplitude = 10.0f,
        int octaves = 4,
        float persistence = 0.5f
    );

    /**
     * @brief Smooth height field using average filter
     * @param heights Height data (row-major)
     * @param width Terrain width
     * @param depth Terrain depth
     * @param iterations Number of smoothing passes
     * @return Smoothed heights
     * 
     * Applies 3x3 average filter to reduce sharp features.
     */
    static std::vector<float> SmoothHeightField(
        const std::vector<float> &heights,
        uint32_t width, uint32_t depth,
        int iterations = 1
    );

    /**
     * @brief Flatten area of terrain
     * @param heights Height data (modified in-place)
     * @param width Terrain width
     * @param depth Terrain depth
     * @param centerX Center X index
     * @param centerZ Center Z index
     * @param radius Flatten radius (vertices)
     * @param targetHeight Height to flatten to
     * 
     * Creates flat circular area (useful for building foundations).
     */
    static void FlattenArea(
        std::vector<float> &heights,
        uint32_t width, uint32_t depth,
        uint32_t centerX, uint32_t centerZ,
        uint32_t radius,
        float targetHeight
    );

    /**
     * @brief Calculate terrain normals
     * @param heights Height data
     * @param width Terrain width
     * @param depth Terrain depth
     * @param scale Terrain scale
     * @return Normal vectors (one per vertex)
     * 
     * Useful for rendering terrain with proper lighting.
     */
    static std::vector<glm::vec3> CalculateNormals(
        const std::vector<float> &heights,
        uint32_t width, uint32_t depth,
        const glm::vec3 &scale = glm::vec3(1.0f)
    );

    /**
     * @brief Downsample height field (reduce resolution)
     * @param heights Height data
     * @param width Original width
     * @param depth Original depth
     * @param factor Downsample factor (2 = half resolution)
     * @return Downsampled heights and new dimensions
     * 
     * @example
     * @code
     * // Reduce 1024x1024 terrain to 256x256 for LOD
     * auto [lowResHeights, newW, newD] = Physics::Resource::TerrainProcessor::Downsample(
     *     highResHeights, 1024, 1024, 4
     * );
     * @endcode
     */
    static std::tuple<std::vector<float>, uint32_t, uint32_t> Downsample(
        const std::vector<float> &heights,
        uint32_t width, uint32_t depth,
        uint32_t factor
    );

private:
    /// Perlin noise implementation
    static float PerlinNoise(float x, float y);
    static float Fade(float t);
    static float Lerp(float t, float a, float b);
    static float Grad(int hash, float x, float y);
};

} // namespace Physics::Resource
```

### Update Shape Dispatcher

```cpp
JPH::ShapeSettings* GetColliderShape(entt::registry &registry, entt::entity entity) {
    // Priority: HeightField > Compound > Box > Sphere > Capsule > Mesh > Default
    
    // 1. Try HeightFieldCollider (NEW - highest priority for terrain)
    if (registry.all_of<Component::HeightFieldCollider>(entity)) {
        auto &heightField = registry.get<Component::HeightFieldCollider>(entity);
        
        if (!heightField.IsValid()) {
            Logger::Error("HeightFieldCollider: Invalid dimensions or empty data");
            return CreateDefaultShape(registry, entity);
        }
        
        // Check motion type (must be static)
        if (registry.all_of<Component::RigidBody>(entity)) {
            auto &rb = registry.get<Component::RigidBody>(entity);
            if (rb.motionType != MotionType::Static) {
                Logger::Error("HeightFieldCollider: Only static bodies supported");
                return CreateDefaultShape(registry, entity);
            }
        }
        
        // Create Jolt height field shape
        JPH::HeightFieldShapeSettings heightFieldSettings(
            heightField.heights.data(),
            JPH::Vec3::sZero(),  // Offset
            JPH::Vec3(heightField.scale.x, heightField.scale.y, heightField.scale.z),
            heightField.width,
            heightField.blockSize
        );
        
        return &heightFieldSettings;
    }
    
    // ... existing colliders ...
}
```

## 📝 Implementation Tasks

### Phase 1: Create HeightFieldCollider Component (3-4 hours)

- [ ] Create `component/HeightFieldCollider.hpp`
- [ ] Implement HeightFieldCollider struct
- [ ] Add height data storage (std::vector<float>)
- [ ] Add width, depth, scale fields
- [ ] Implement CreateFlat() factory
- [ ] Implement CreateFromImage() factory
- [ ] Add GetHeight() / SetHeight() methods
- [ ] Add GetHeightAt() interpolation
- [ ] Add validation methods
- [ ] Add comprehensive Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Create TerrainProcessor Resource (5-6 hours)

- [ ] Create `resource/TerrainProcessor.hpp`
- [ ] Implement Perlin noise generation
  - Gradient noise algorithm
  - Octave layering
  - Configurable scale/amplitude
- [ ] Implement SmoothHeightField()
  - 3x3 average filter
  - Multiple iterations
- [ ] Implement FlattenArea()
  - Circular area
  - Smooth falloff
- [ ] Implement CalculateNormals()
  - Finite differences
  - Normalization
- [ ] Implement Downsample()
  - LOD support
- [ ] Add unit tests

### Phase 3: Update RigidBodySystem (3-4 hours)

- [ ] Update shape dispatcher in `system/RigidBodySystem.cpp`
- [ ] Add HeightFieldCollider case (highest priority)
- [ ] Validate motion type (static only)
- [ ] Create JPH::HeightFieldShapeSettings
- [ ] Convert height data to Jolt format
- [ ] Set scale and block size
- [ ] Handle invalid height field error

### Phase 4: Image Loading Integration (4-5 hours)

- [ ] Implement CreateFromImage()
- [ ] Use Object::Resource::ImageLoader (or stb_image)
- [ ] Convert grayscale to height values
- [ ] Apply height scale multiplier
- [ ] Add error handling for missing/invalid images
- [ ] Support PNG/JPG/TGA formats

### Phase 5: Unit Tests (5-6 hours)

- [ ] Create `tests/HeightFieldColliderTest.cpp`
- [ ] Test flat terrain creation
- [ ] Test height get/set
- [ ] Test height interpolation
- [ ] Test bounds calculation
- [ ] Test image loading
- [ ] Test validation
- [ ] Create `tests/TerrainProcessorTest.cpp`
- [ ] Test Perlin noise generation
- [ ] Test smoothing
- [ ] Test flattening
- [ ] Test normal calculation
- [ ] Test downsampling

### Phase 6: Integration Examples (4-5 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Flat Ground" example
  - Simple 100x100 flat terrain
- [ ] Add "Perlin Terrain" example
  - Procedurally generated hills
- [ ] Add "Height Map" example
  - Load terrain from image
- [ ] Add "Terrain Modification" example
  - Flatten area for building

### Phase 7: Documentation (2-3 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Terrain Collision (HeightField)" section
  - Performance benefits vs MeshCollider
  - When to use HeightField vs Mesh
- [ ] Add usage examples
- [ ] Document limitations (static only, no overhangs)

## 🧪 Testing Checklist

### Unit Tests

- [ ] Empty height field is rejected
- [ ] Valid height field is accepted
- [ ] GetHeight() returns correct values
- [ ] SetHeight() updates correctly
- [ ] GetHeightAt() interpolates smoothly
- [ ] Bounds calculation is accurate
- [ ] Memory estimation is reasonable
- [ ] CreateFlat() works
- [ ] CreateFromImage() loads correctly

### TerrainProcessor Tests

- [ ] Perlin noise generates reasonable terrain
- [ ] Smoothing reduces sharp features
- [ ] Flattening creates flat areas
- [ ] Normal calculation is correct
- [ ] Downsampling preserves shape

### Integration Tests

- [ ] Flat ground supports dynamic objects
- [ ] Hilly terrain collides correctly
- [ ] Sphere rolls down slopes realistically
- [ ] Objects rest on terrain
- [ ] Large terrains (1000x1000) perform well

### Performance Tests

- [ ] 256x256 terrain: < 1ms creation time
- [ ] 1024x1024 terrain: < 10ms creation time
- [ ] Height lookup: < 0.001ms (O(1))
- [ ] 1000 objects on terrain: < 5ms per frame

### Edge Cases

- [ ] Dynamic body rejected (error)
- [ ] Zero-size terrain rejected
- [ ] Very large terrain (4096x4096)
- [ ] Non-power-of-2 dimensions
- [ ] Negative height values

## 📚 Code Examples

### Example 1: Simple Flat Ground

```cpp
auto ground = core.CreateEntity();

// Transform
ground.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

// Physics: static body (required for height field)
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
ground.AddComponent<Physics::Component::RigidBody>(core, rb);

// Height field: 100x100 flat at Y=0
auto heightField = Physics::Component::HeightFieldCollider::CreateFlat(
    100, 100, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f)
);
ground.AddComponent<Physics::Component::HeightFieldCollider>(core, heightField);
```

### Example 2: Procedural Perlin Terrain

```cpp
auto terrain = core.CreateEntity();

// Generate height data
auto heights = Physics::Resource::TerrainProcessor::GeneratePerlinTerrain(
    256, 256,   // Resolution
    50.0f,      // Scale (smoothness)
    20.0f,      // Amplitude (max height)
    4,          // Octaves (detail layers)
    0.5f        // Persistence (roughness)
);

// Smooth terrain
heights = Physics::Resource::TerrainProcessor::SmoothHeightField(
    heights, 256, 256, 2  // 2 smoothing passes
);

// Transform
terrain.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

// Physics: static body
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
terrain.AddComponent<Physics::Component::RigidBody>(core, rb);

// Height field collider
Physics::Component::HeightFieldCollider heightField(
    256, 256, heights, glm::vec3(1.0f, 1.0f, 1.0f)
);
terrain.AddComponent<Physics::Component::HeightFieldCollider>(core, heightField);
```

### Example 3: Load Terrain from Image

```cpp
auto terrain = core.CreateEntity();

// Transform
terrain.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

// Physics: static body
Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
terrain.AddComponent<Physics::Component::RigidBody>(core, rb);

// Height field from image
// Grayscale values map to height (black=0, white=50)
auto heightField = Physics::Component::HeightFieldCollider::CreateFromImage(
    "assets/terrain_heightmap.png",
    50.0f,  // Max height
    glm::vec3(1.0f, 1.0f, 1.0f)
);
terrain.AddComponent<Physics::Component::HeightFieldCollider>(core, heightField);
```

### Example 4: Terrain with Flattened Building Area

```cpp
// Generate terrain
auto heights = Physics::Resource::TerrainProcessor::GeneratePerlinTerrain(
    256, 256, 50.0f, 20.0f, 4, 0.5f
);

// Flatten center for building
Physics::Resource::TerrainProcessor::FlattenArea(
    heights, 256, 256,
    128, 128,  // Center
    20,        // Radius (20 vertices)
    5.0f       // Target height
);

// Create terrain entity
auto terrain = core.CreateEntity();
terrain.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
terrain.AddComponent<Physics::Component::RigidBody>(core, rb);

Physics::Component::HeightFieldCollider heightField(
    256, 256, heights, glm::vec3(1.0f, 1.0f, 1.0f)
);
terrain.AddComponent<Physics::Component::HeightFieldCollider>(core, heightField);

// Place building on flattened area
auto building = core.CreateEntity();
building.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 5, 0)));
// ... building components ...
```

### Example 5: Multi-Resolution Terrain (LOD)

```cpp
// High-res terrain for physics (256x256)
auto highResHeights = Physics::Resource::TerrainProcessor::GeneratePerlinTerrain(
    256, 256, 50.0f, 20.0f, 4, 0.5f
);

auto physicsTerrain = core.CreateEntity();
physicsTerrain.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(0, 0, 0)));

Physics::Component::RigidBody rb = Physics::Component::RigidBody::CreateStatic();
physicsTerrain.AddComponent<Physics::Component::RigidBody>(core, rb);

Physics::Component::HeightFieldCollider physicsHeightField(
    256, 256, highResHeights, glm::vec3(1.0f, 1.0f, 1.0f)
);
physicsTerrain.AddComponent<Physics::Component::HeightFieldCollider>(core, physicsHeightField);

// Low-res for distant rendering (64x64)
auto [lowResHeights, lowW, lowD] = Physics::Resource::TerrainProcessor::Downsample(
    highResHeights, 256, 256, 4  // 4x downsampling
);

// Use lowResHeights for distant terrain mesh rendering
// ... (rendering code) ...
```

### Example 6: Query Terrain Height at Runtime

```cpp
// Get height field component
auto &heightField = terrain.GetComponent<Physics::Component::HeightFieldCollider>(core);

// Query height at world position
glm::vec3 queryPos(10.0f, 0.0f, 15.0f);
float terrainHeight = heightField.GetHeightAt(queryPos.x, queryPos.z);

// Place object on terrain surface
auto object = core.CreateEntity();
object.AddComponent<Object::Component::Transform>(core,
    Object::Component::Transform::Create(glm::vec3(queryPos.x, terrainHeight + 1.0f, queryPos.z)));
```

## 🐛 Error Handling

### Cases to Handle

1. **Non-static body**
   ```cpp
   if (rb.motionType != MotionType::Static) {
       Logger::Error("HeightFieldCollider: Only static motion type supported (use MeshCollider for dynamic terrain chunks)");
       return nullptr;
   }
   ```

2. **Invalid dimensions**
   ```cpp
   if (width < 2 || depth < 2) {
       Logger::Error("HeightFieldCollider: Minimum dimensions are 2x2");
       return nullptr;
   }
   ```

3. **Data size mismatch**
   ```cpp
   if (heights.size() != width * depth) {
       Logger::Error("HeightFieldCollider: Height data size ({}) doesn't match dimensions ({}x{})",
                     heights.size(), width, depth);
       return nullptr;
   }
   ```

4. **Image loading failure**
   ```cpp
   if (!imageData) {
       Logger::Error("HeightFieldCollider: Failed to load heightmap image: {}", imagePath);
       return HeightFieldCollider();  // Empty/invalid
   }
   ```

5. **Very large terrain warning**
   ```cpp
   if (width * depth > 2048 * 2048) {
       Logger::Warn("HeightFieldCollider: Very large terrain ({}x{}) may impact memory and loading time",
                    width, depth);
   }
   ```

## 💡 Design Considerations

### HeightField vs MeshCollider

| Aspect | HeightField | MeshCollider (Triangle Mesh) |
|--------|-------------|------------------------------|
| **Performance** | Very fast (O(1) lookup) | Slower (BVH traversal) |
| **Memory** | 1 float/vertex | 3 floats/vertex + indices |
| **Size Limit** | 4096x4096+ | ~500 triangles optimal |
| **Overhangs** | ❌ No | ✅ Yes |
| **Caves** | ❌ No | ✅ Yes |
| **Dynamic** | ❌ Static only | ✅ Static or dynamic chunks |
| **Use Case** | Large open terrain | Complex geometry, caves |

**Rule of thumb:**
- Use **HeightField** for: outdoor terrain, flat surfaces, landscapes
- Use **MeshCollider** for: caves, buildings, complex shapes with overhangs

### Block Size Optimization

Jolt uses block-based storage for HeightFields:
- **Smaller blocks (2-4)**: Lower memory, slower queries
- **Larger blocks (8-16)**: Higher memory, faster queries
- **Default: 8** (good balance)

### Terrain Scale Considerations

```cpp
// ❌ BAD: Unnatural 1:1:1 scale
glm::vec3(1.0f, 1.0f, 1.0f)  // 1 unit = 1 meter, very steep terrain

// ✅ GOOD: Realistic scale
glm::vec3(2.0f, 0.5f, 2.0f)  // Stretch XZ, compress Y = gentle slopes
```

### Runtime Modification

HeightFields are **immutable** in Jolt. To modify terrain:
1. Update `heights` data
2. Call `SetHeight(x, z, newHeight)`
3. **Destroy and recreate** the RigidBody

For frequently changing terrain, consider:
- Using dynamic MeshCollider chunks
- Editing only during loading/setup
- Pre-computing multiple terrain states

## 📊 Success Criteria

- [ ] HeightFieldCollider component implemented
- [ ] TerrainProcessor resource implemented
- [ ] Image loading works (PNG/JPG)
- [ ] Perlin noise generation works
- [ ] Smoothing and flattening work
- [ ] Normal calculation correct
- [ ] RigidBodySystem handles HeightField
- [ ] Static-only validation enforced
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 10ms for 1024x1024)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt HeightFieldShape: https://jrouwe.github.io/JoltPhysics/class_height_field_shape.html
- Unity Terrain Collider: https://docs.unity3d.com/Manual/class-TerrainCollider.html
- Godot HeightMapShape: https://docs.godotengine.org/en/stable/classes/class_heightmapshape3d.html
- Perlin Noise: https://en.wikipedia.org/wiki/Perlin_noise

## 🚀 Follow-up Issues

- #015: Runtime terrain modification (mutable height fields)
- #016: Terrain LOD system (multiple resolutions)
- #017: Procedural terrain generation (advanced noise, erosion)
- #025: Terrain painting/sculpting tools (editor integration)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.4`, `priority:high`, `terrain`  
**Milestone:** v0.4 - Collider Expansion
