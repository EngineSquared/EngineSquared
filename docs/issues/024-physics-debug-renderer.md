# Issue #024: Implement Physics Debug Visualization (Wireframe Rendering)

**Milestone:** v0.8 - Ecosystem & Tools  
**Priority:** 🔴 HIGH  
**Estimated Effort:** 4-5 days  
**Dependencies:** None (standalone, optional #023 integration)  
**Related Issues:** #023 (Physics Profiler), #026 (Editor Integration)  
**Follow-up Issues:** None

---

## 📋 Description

Implement comprehensive debug visualization system for physics objects. Render wireframes, collision shapes, contact points, forces, velocities, constraints, and sensors. Essential for debugging physics issues and understanding simulation behavior.

## 🎯 Goals

- ✅ Implement PhysicsDebugRenderer resource
- ✅ Render collision shapes (wireframe)
- ✅ Render contact points and normals
- ✅ Render forces and velocities (arrows)
- ✅ Render constraints (lines, pivots)
- ✅ Render sensors (transparent volumes)
- ✅ Render raycasts and queries
- ✅ Color coding (kinematic, sleeping, active)
- ✅ Toggle individual layers
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── PhysicsDebugRenderer.hpp    # New file
│   └── PhysicsDebugRenderer.cpp    # New file
├── system/
│   ├── PhysicsDebugSystem.hpp      # New file
│   └── PhysicsDebugSystem.cpp      # New file
└── tests/
    └── PhysicsDebugTest.cpp        # New file
```

### PhysicsDebugRenderer Resource

```cpp
/**************************************************************************
 * @file PhysicsDebugRenderer.hpp
 * @brief Physics debug renderer (wireframe visualization)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Debug render flags (what to render)
 */
enum class DebugRenderFlags : uint32_t {
    None            = 0,
    Shapes          = 1 << 0,   ///< Collision shapes (wireframe)
    Contacts        = 1 << 1,   ///< Contact points + normals
    ContactNormals  = 1 << 2,   ///< Contact normals (arrows)
    Velocities      = 1 << 3,   ///< Linear velocities (arrows)
    Forces          = 1 << 4,   ///< Applied forces (arrows)
    Constraints     = 1 << 5,   ///< Constraints (lines, pivots)
    Sensors         = 1 << 6,   ///< Sensors (transparent volumes)
    Raycasts        = 1 << 7,   ///< Raycasts (lines)
    AABB            = 1 << 8,   ///< Axis-aligned bounding boxes
    CenterOfMass    = 1 << 9,   ///< Center of mass (points)
    Axes            = 1 << 10,  ///< Local axes (RGB = XYZ)
    
    All = Shapes | Contacts | ContactNormals | Velocities | 
          Constraints | Sensors | AABB | CenterOfMass | Axes
};

// Bitwise operators
inline DebugRenderFlags operator|(DebugRenderFlags a, DebugRenderFlags b) {
    return static_cast<DebugRenderFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline DebugRenderFlags operator&(DebugRenderFlags a, DebugRenderFlags b) {
    return static_cast<DebugRenderFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool HasFlag(DebugRenderFlags flags, DebugRenderFlags flag) {
    return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
}

/**
 * @brief Debug render colors
 */
struct DebugColors {
    glm::vec4 dynamic = glm::vec4(0, 1, 0, 1);      ///< Green
    glm::vec4 kinematic = glm::vec4(0, 0, 1, 1);    ///< Blue
    glm::vec4 staticBody = glm::vec4(1, 1, 1, 1);   ///< White
    glm::vec4 sleeping = glm::vec4(0.5f, 0.5f, 0.5f, 1);  ///< Gray
    glm::vec4 contact = glm::vec4(1, 0, 0, 1);      ///< Red
    glm::vec4 constraint = glm::vec4(1, 1, 0, 1);   ///< Yellow
    glm::vec4 sensor = glm::vec4(0, 1, 1, 0.3f);    ///< Cyan (transparent)
    glm::vec4 raycast = glm::vec4(1, 0, 1, 1);      ///< Magenta
    
    /**
     * @brief Create default colors
     * @return DebugColors
     */
    static DebugColors Default() {
        return {};
    }
};

/**
 * @brief Debug render settings
 */
struct DebugRenderSettings {
    /// Flags
    DebugRenderFlags flags = DebugRenderFlags::Shapes;
    
    /// Colors
    DebugColors colors;
    
    /// Sizes
    float contactPointSize = 0.1f;          ///< Contact point sphere radius
    float arrowLength = 1.0f;               ///< Velocity/force arrow length scale
    float axisLength = 0.5f;                ///< Local axes length
    float lineThickness = 2.0f;             ///< Line thickness (pixels)
    
    /// Filters
    bool renderStaticBodies = true;         ///< Render static bodies
    bool renderSleepingBodies = true;       ///< Render sleeping bodies
    
    /**
     * @brief Create default settings
     * @return DebugRenderSettings
     */
    static DebugRenderSettings Default() {
        return {};
    }
    
    /**
     * @brief Create minimal settings (shapes only)
     * @return DebugRenderSettings
     */
    static DebugRenderSettings Minimal() {
        DebugRenderSettings s;
        s.flags = DebugRenderFlags::Shapes;
        s.renderStaticBodies = false;
        return s;
    }
    
    /**
     * @brief Create full settings (everything)
     * @return DebugRenderSettings
     */
    static DebugRenderSettings Full() {
        DebugRenderSettings s;
        s.flags = DebugRenderFlags::All;
        return s;
    }
};

/**
 * @brief Physics debug renderer
 * 
 * Renders physics debug visualization (wireframes, contacts, forces, etc.)
 * 
 * **Features:**
 * - Collision shapes (wireframe boxes, spheres, meshes)
 * - Contact points and normals
 * - Velocities and forces (arrows)
 * - Constraints (lines, pivots)
 * - Sensors (transparent volumes)
 * - Raycasts (lines with hit points)
 * - Color coding (kinematic, sleeping, active)
 * 
 * @example
 * @code
 * // Enable debug rendering
 * auto settings = Physics::Resource::DebugRenderSettings::Default();
 * settings.flags = Physics::Resource::DebugRenderFlags::Shapes | 
 *                  Physics::Resource::DebugRenderFlags::Contacts;
 * Physics::Resource::PhysicsDebugRenderer::Enable(core, settings);
 * 
 * // In render loop
 * Physics::Resource::PhysicsDebugRenderer::Render(core);
 * 
 * // Toggle specific feature
 * Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
 *     Physics::Resource::DebugRenderFlags::Velocities, true);
 * @endcode
 */
class PhysicsDebugRenderer {
public:
    /**
     * @brief Enable debug renderer
     * @param core Engine core
     * @param settings Debug render settings
     */
    static void Enable(Engine::Core &core, const DebugRenderSettings &settings = DebugRenderSettings::Default());
    
    /**
     * @brief Disable debug renderer
     * @param core Engine core
     */
    static void Disable(Engine::Core &core);
    
    /**
     * @brief Check if debug renderer is enabled
     * @param core Engine core
     * @return true if enabled
     */
    static bool IsEnabled(Engine::Core &core);
    
    /**
     * @brief Render debug visualization
     * @param core Engine core
     * 
     * Call this in render loop after physics update.
     */
    static void Render(Engine::Core &core);
    
    /**
     * @brief Set render flag
     * @param core Engine core
     * @param flag Flag to set
     * @param enabled true to enable, false to disable
     */
    static void SetFlag(Engine::Core &core, DebugRenderFlags flag, bool enabled);
    
    /**
     * @brief Get render flags
     * @param core Engine core
     * @return Current flags
     */
    static DebugRenderFlags GetFlags(Engine::Core &core);
    
    /**
     * @brief Set render color
     * @param core Engine core
     * @param type Color type (dynamic, kinematic, etc.)
     * @param color New color
     */
    static void SetColor(Engine::Core &core, const std::string &type, const glm::vec4 &color);
    
    /**
     * @brief Get settings
     * @param core Engine core
     * @return Current settings
     */
    static DebugRenderSettings GetSettings(Engine::Core &core);
    
    /**
     * @brief Set settings
     * @param core Engine core
     * @param settings New settings
     */
    static void SetSettings(Engine::Core &core, const DebugRenderSettings &settings);
    
    /**
     * @brief Draw custom line (persistent until next frame)
     * @param core Engine core
     * @param start Start position
     * @param end End position
     * @param color Line color
     */
    static void DrawLine(Engine::Core &core, const glm::vec3 &start, const glm::vec3 &end, const glm::vec4 &color = glm::vec4(1));
    
    /**
     * @brief Draw custom arrow
     * @param core Engine core
     * @param start Start position
     * @param direction Direction vector
     * @param color Arrow color
     */
    static void DrawArrow(Engine::Core &core, const glm::vec3 &start, const glm::vec3 &direction, const glm::vec4 &color = glm::vec4(1));
    
    /**
     * @brief Draw custom sphere (wireframe)
     * @param core Engine core
     * @param center Center position
     * @param radius Radius
     * @param color Sphere color
     */
    static void DrawSphere(Engine::Core &core, const glm::vec3 &center, float radius, const glm::vec4 &color = glm::vec4(1));
    
    /**
     * @brief Draw custom box (wireframe)
     * @param core Engine core
     * @param center Center position
     * @param extents Half-extents
     * @param rotation Rotation quaternion
     * @param color Box color
     */
    static void DrawBox(Engine::Core &core, const glm::vec3 &center, const glm::vec3 &extents, const glm::quat &rotation, const glm::vec4 &color = glm::vec4(1));
    
    /**
     * @brief Clear custom debug shapes
     * @param core Engine core
     */
    static void ClearCustomShapes(Engine::Core &core);
    
private:
    /// Internal state
    struct State {
        bool enabled = false;
        DebugRenderSettings settings;
        std::vector<std::pair<glm::vec3, glm::vec3>> customLines;
        std::vector<std::tuple<glm::vec3, glm::vec3, glm::vec4>> customArrows;
    };
    
    static State& GetState(Engine::Core &core);
    
    /// Render functions
    static void RenderShapes(Engine::Core &core);
    static void RenderContacts(Engine::Core &core);
    static void RenderVelocities(Engine::Core &core);
    static void RenderConstraints(Engine::Core &core);
    static void RenderSensors(Engine::Core &core);
    static void RenderCustomShapes(Engine::Core &core);
    
    /// Helper functions
    static glm::vec4 GetBodyColor(Engine::Core &core, Engine::Entity entity);
    static void DrawWireframeSphere(const glm::vec3 &center, float radius, const glm::vec4 &color);
    static void DrawWireframeBox(const glm::vec3 &center, const glm::vec3 &extents, const glm::quat &rotation, const glm::vec4 &color);
    static void DrawWireframeCapsule(const glm::vec3 &center, float radius, float height, const glm::quat &rotation, const glm::vec4 &color);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create PhysicsDebugRenderer (8-10 hours)

- [ ] Create `resource/PhysicsDebugRenderer.hpp`
- [ ] Implement DebugRenderFlags enum
- [ ] Implement DebugColors struct
- [ ] Implement DebugRenderSettings struct
- [ ] Implement Enable/Disable
- [ ] Implement SetFlag/GetFlags
- [ ] Implement DrawLine/DrawArrow/DrawSphere/DrawBox
- [ ] Add Doxygen comments

### Phase 2: Implement Render Functions (12-14 hours)

- [ ] Implement RenderShapes()
  - Box colliders (wireframe)
  - Sphere colliders (wireframe)
  - Capsule colliders (wireframe)
  - Mesh colliders (wireframe edges)
- [ ] Implement RenderContacts()
  - Contact points (small spheres)
  - Contact normals (arrows)
- [ ] Implement RenderVelocities()
  - Linear velocity (arrows)
  - Angular velocity (curved arrows)
- [ ] Implement RenderConstraints()
  - Distance constraints (lines)
  - Hinge constraints (pivot + axis)
  - 6DOF constraints (full axes)
- [ ] Implement RenderSensors()
  - Sphere detectors (transparent spheres)
  - Box detectors (transparent boxes)
  - Cone detectors (transparent cones)

### Phase 3: Integrate with Graphic Plugin (6-8 hours)

- [ ] Create PhysicsDebugSystem
- [ ] Implement Update() (collect debug data)
- [ ] Implement Render() (draw using Graphic plugin)
- [ ] Use line renderer / wireframe shader
- [ ] Batch draw calls for performance

### Phase 4: Color Coding (3-4 hours)

- [ ] Implement GetBodyColor()
  - Dynamic: Green
  - Kinematic: Blue
  - Static: White
  - Sleeping: Gray
- [ ] Apply colors to shapes
- [ ] Allow custom colors

### Phase 5: Unit Tests (6-8 hours)

- [ ] Create `tests/PhysicsDebugTest.cpp`
- [ ] Test Enable/Disable
- [ ] Test flag toggling
- [ ] Test color settings
- [ ] Test custom shapes
- [ ] Test rendering (mock)

### Phase 6: Integration Examples (8-10 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add debug visualization toggle (F3 key)
- [ ] Add UI to toggle individual flags
- [ ] Add color picker for customization
- [ ] Add examples for each feature

### Phase 7: Documentation (4-5 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Debug Visualization" section
  - Explain flags and colors
  - Explain custom shapes
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Renderer enables/disables correctly
- [ ] Flags toggle correctly
- [ ] Colors set correctly
- [ ] Custom shapes work
- [ ] Clearing works

### Integration Tests

- [ ] Shapes render correctly
- [ ] Contacts visible
- [ ] Velocities visible
- [ ] Constraints visible
- [ ] Sensors visible
- [ ] Colors correct

### Performance Tests

- [ ] Render 1000 shapes: < 5ms
- [ ] Minimal overhead when disabled

### Edge Cases

- [ ] No physics entities
- [ ] All flags disabled
- [ ] Very complex mesh collider
- [ ] Overlapping shapes

## 📚 Code Examples

### Example 1: Basic Debug Rendering

```cpp
// Enable debug rendering (shapes only)
auto settings = Physics::Resource::DebugRenderSettings::Minimal();
Physics::Resource::PhysicsDebugRenderer::Enable(core, settings);

// Game loop
while (running) {
    // Update physics
    core.Update();
    
    // Render debug visualization
    Physics::Resource::PhysicsDebugRenderer::Render(core);
    
    // Render normal graphics
    Render(core);
}

// Disable when done
Physics::Resource::PhysicsDebugRenderer::Disable(core);
```

### Example 2: Toggle Debug Features with Keys

```cpp
// Toggle debug features with F-keys
if (Input::IsKeyPressed(Input::Key::F1)) {
    auto flags = Physics::Resource::PhysicsDebugRenderer::GetFlags(core);
    bool enabled = HasFlag(flags, Physics::Resource::DebugRenderFlags::Shapes);
    Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
        Physics::Resource::DebugRenderFlags::Shapes, !enabled);
    Logger::Info("Shapes: {}", !enabled ? "ON" : "OFF");
}

if (Input::IsKeyPressed(Input::Key::F2)) {
    auto flags = Physics::Resource::PhysicsDebugRenderer::GetFlags(core);
    bool enabled = HasFlag(flags, Physics::Resource::DebugRenderFlags::Contacts);
    Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
        Physics::Resource::DebugRenderFlags::Contacts, !enabled);
    Logger::Info("Contacts: {}", !enabled ? "ON" : "OFF");
}

if (Input::IsKeyPressed(Input::Key::F3)) {
    auto flags = Physics::Resource::PhysicsDebugRenderer::GetFlags(core);
    bool enabled = HasFlag(flags, Physics::Resource::DebugRenderFlags::Velocities);
    Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
        Physics::Resource::DebugRenderFlags::Velocities, !enabled);
    Logger::Info("Velocities: {}", !enabled ? "ON" : "OFF");
}

if (Input::IsKeyPressed(Input::Key::F4)) {
    auto flags = Physics::Resource::PhysicsDebugRenderer::GetFlags(core);
    bool enabled = HasFlag(flags, Physics::Resource::DebugRenderFlags::Constraints);
    Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
        Physics::Resource::DebugRenderFlags::Constraints, !enabled);
    Logger::Info("Constraints: {}", !enabled ? "ON" : "OFF");
}
```

### Example 3: Custom Colors

```cpp
// Customize debug colors
auto settings = Physics::Resource::DebugRenderSettings::Default();

// Make dynamic bodies bright green
settings.colors.dynamic = glm::vec4(0, 1, 0, 1);

// Make static bodies darker
settings.colors.staticBody = glm::vec4(0.5f, 0.5f, 0.5f, 1);

// Make contacts more visible (bright red)
settings.colors.contact = glm::vec4(1, 0, 0, 1);

Physics::Resource::PhysicsDebugRenderer::Enable(core, settings);
```

### Example 4: Draw Custom Debug Shapes

```cpp
// Draw path (for AI pathfinding visualization)
std::vector<glm::vec3> path = {
    glm::vec3(0, 0, 0),
    glm::vec3(5, 0, 5),
    glm::vec3(10, 0, 5),
    glm::vec3(15, 0, 10)
};

for (size_t i = 0; i < path.size() - 1; ++i) {
    Physics::Resource::PhysicsDebugRenderer::DrawLine(
        core, path[i], path[i + 1], glm::vec4(1, 1, 0, 1)  // Yellow
    );
}

// Draw waypoint spheres
for (auto &point : path) {
    Physics::Resource::PhysicsDebugRenderer::DrawSphere(
        core, point, 0.5f, glm::vec4(0, 1, 1, 1)  // Cyan
    );
}

// Draw current direction (arrow)
glm::vec3 playerPos = player.GetComponent<Object::Component::Transform>(core).position;
glm::vec3 targetPos = path[currentWaypoint];
glm::vec3 direction = glm::normalize(targetPos - playerPos);

Physics::Resource::PhysicsDebugRenderer::DrawArrow(
    core, playerPos, direction * 2.0f, glm::vec4(1, 0, 1, 1)  // Magenta
);
```

### Example 5: ImGui Debug Control Panel

```cpp
void RenderPhysicsDebugPanel(Engine::Core &core) {
    if (!ImGui::Begin("Physics Debug")) {
        ImGui::End();
        return;
    }
    
    // Enable/Disable
    bool enabled = Physics::Resource::PhysicsDebugRenderer::IsEnabled(core);
    if (ImGui::Checkbox("Enable Debug Rendering", &enabled)) {
        if (enabled) {
            Physics::Resource::PhysicsDebugRenderer::Enable(core);
        } else {
            Physics::Resource::PhysicsDebugRenderer::Disable(core);
        }
    }
    
    if (!enabled) {
        ImGui::End();
        return;
    }
    
    ImGui::Separator();
    ImGui::Text("Render Flags");
    
    // Toggle flags
    auto flags = Physics::Resource::PhysicsDebugRenderer::GetFlags(core);
    
    bool shapes = HasFlag(flags, Physics::Resource::DebugRenderFlags::Shapes);
    if (ImGui::Checkbox("Shapes", &shapes)) {
        Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
            Physics::Resource::DebugRenderFlags::Shapes, shapes);
    }
    
    bool contacts = HasFlag(flags, Physics::Resource::DebugRenderFlags::Contacts);
    if (ImGui::Checkbox("Contacts", &contacts)) {
        Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
            Physics::Resource::DebugRenderFlags::Contacts, contacts);
    }
    
    bool velocities = HasFlag(flags, Physics::Resource::DebugRenderFlags::Velocities);
    if (ImGui::Checkbox("Velocities", &velocities)) {
        Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
            Physics::Resource::DebugRenderFlags::Velocities, velocities);
    }
    
    bool constraints = HasFlag(flags, Physics::Resource::DebugRenderFlags::Constraints);
    if (ImGui::Checkbox("Constraints", &constraints)) {
        Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
            Physics::Resource::DebugRenderFlags::Constraints, constraints);
    }
    
    bool sensors = HasFlag(flags, Physics::Resource::DebugRenderFlags::Sensors);
    if (ImGui::Checkbox("Sensors", &sensors)) {
        Physics::Resource::PhysicsDebugRenderer::SetFlag(core, 
            Physics::Resource::DebugRenderFlags::Sensors, sensors);
    }
    
    ImGui::Separator();
    ImGui::Text("Colors");
    
    // Color pickers
    auto settings = Physics::Resource::PhysicsDebugRenderer::GetSettings(core);
    
    if (ImGui::ColorEdit4("Dynamic", &settings.colors.dynamic.r)) {
        Physics::Resource::PhysicsDebugRenderer::SetSettings(core, settings);
    }
    
    if (ImGui::ColorEdit4("Kinematic", &settings.colors.kinematic.r)) {
        Physics::Resource::PhysicsDebugRenderer::SetSettings(core, settings);
    }
    
    if (ImGui::ColorEdit4("Static", &settings.colors.staticBody.r)) {
        Physics::Resource::PhysicsDebugRenderer::SetSettings(core, settings);
    }
    
    ImGui::End();
}
```

### Example 6: Visualize Raycast Results

```cpp
// Perform raycast
glm::vec3 origin = player.GetComponent<Object::Component::Transform>(core).position;
glm::vec3 direction = player.GetComponent<Object::Component::Transform>(core).GetForward();

auto hit = Physics::Resource::RaycastQuery::Cast(core, origin, direction, 100.0f);

// Draw raycast line
if (hit.hit) {
    // Hit something - draw line to hit point (red)
    Physics::Resource::PhysicsDebugRenderer::DrawLine(
        core, origin, hit.point, glm::vec4(1, 0, 0, 1)
    );
    
    // Draw hit normal (blue arrow)
    Physics::Resource::PhysicsDebugRenderer::DrawArrow(
        core, hit.point, hit.normal * 0.5f, glm::vec4(0, 0, 1, 1)
    );
    
    // Draw hit point (sphere)
    Physics::Resource::PhysicsDebugRenderer::DrawSphere(
        core, hit.point, 0.1f, glm::vec4(1, 1, 0, 1)
    );
} else {
    // No hit - draw full line (green)
    Physics::Resource::PhysicsDebugRenderer::DrawLine(
        core, origin, origin + direction * 100.0f, glm::vec4(0, 1, 0, 1)
    );
}
```

## 🐛 Error Handling

### Cases to Handle

1. **Renderer not enabled**
   ```cpp
   if (!PhysicsDebugRenderer::IsEnabled(core)) {
       // Don't render, no error
       return;
   }
   ```

2. **Invalid color**
   ```cpp
   if (color.a < 0.0f || color.a > 1.0f) {
       Logger::Warning("PhysicsDebugRenderer: Invalid alpha {}, clamping", color.a);
       color.a = glm::clamp(color.a, 0.0f, 1.0f);
   }
   ```

3. **Missing graphic plugin**
   ```cpp
   if (!core.HasPlugin<Graphic::PluginGraphic>()) {
       Logger::Error("PhysicsDebugRenderer: Graphic plugin not loaded");
       return;
   }
   ```

## 💡 Design Considerations

### Wireframe Rendering

- Use line renderer (GL_LINES)
- Batch draw calls for performance
- Use simple shader (position + color)

### Performance

- Only render visible objects (frustum culling)
- LOD: Simpler wireframes for distant objects
- Render at lower frequency (e.g., 30 Hz instead of 60 Hz)

### Integration with Graphic Plugin

```cpp
// Use Graphic plugin line renderer
Graphic::Resource::LineRenderer::DrawLine(start, end, color);
Graphic::Resource::LineRenderer::DrawWireframeSphere(center, radius, color);
```

## 📊 Success Criteria

- [ ] PhysicsDebugRenderer resource implemented
- [ ] Shapes render correctly (box, sphere, capsule, mesh)
- [ ] Contacts render correctly
- [ ] Velocities render correctly
- [ ] Constraints render correctly
- [ ] Sensors render correctly
- [ ] Custom shapes work
- [ ] Color coding works
- [ ] Flag toggling works
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples demonstrate usage
- [ ] Performance acceptable (< 5ms for 1000 shapes)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Debug Rendering: https://github.com/jrouwe/JoltPhysics/blob/master/Samples/Tests/General/DebugRendererTest.cpp
- Unity Gizmos: https://docs.unity3d.com/Manual/GizmosAndHandles.html
- Unreal Debug Drawing: https://unrealcommunity.wiki/debug-drawing-api-84iy7d0f

## 🚀 Follow-up Issues

- #026: Editor Integration (integrate debug renderer in editor)
- #028: Line Renderer (optimize line drawing)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.8`, `priority:high`, `debug`, `visualization`, `tools`  
**Milestone:** v0.8 - Ecosystem & Tools
