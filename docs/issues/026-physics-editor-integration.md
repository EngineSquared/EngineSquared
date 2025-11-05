# Issue #026: Implement Editor Integration (Gizmos, Inspectors, Tools)

**Milestone:** v0.8 - Ecosystem & Tools  
**Priority:** 🟡 MEDIUM  
**Estimated Effort:** 5-6 days  
**Dependencies:** #024 (Debug Visualization for gizmos), #025 (Serialization for save/load)  
**Related Issues:** #023 (Physics Profiler for inspector metrics)  
**Follow-up Issues:** None

---

## 📋 Description

Implement comprehensive editor integration for physics plugin. Add interactive gizmos for editing colliders/constraints, property inspectors, manipulation tools, and real-time preview. Essential for level design and physics tuning.

## 🎯 Goals

- ✅ Implement PhysicsEditorTools resource
- ✅ Interactive gizmos (move, rotate, scale colliders)
- ✅ Property inspectors (edit component properties)
- ✅ Collider creation tools (box, sphere, capsule, etc.)
- ✅ Constraint creation tools (connect entities)
- ✅ Real-time preview (simulate physics in editor)
- ✅ Undo/Redo support
- ✅ Snap to grid/surface
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── resource/
│   ├── PhysicsEditorTools.hpp      # New file
│   └── PhysicsEditorTools.cpp      # New file
├── system/
│   ├── PhysicsEditorSystem.hpp     # New file
│   └── PhysicsEditorSystem.cpp     # New file
└── tests/
    └── PhysicsEditorTest.cpp       # New file
```

### PhysicsEditorTools Resource

```cpp
/**************************************************************************
 * @file PhysicsEditorTools.hpp
 * @brief Physics editor tools (gizmos, inspectors)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <functional>

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Editor tool type
 */
enum class EditorTool {
    None,           ///< No tool (select only)
    Move,           ///< Move tool (translate)
    Rotate,         ///< Rotate tool
    Scale,          ///< Scale tool
    CreateBox,      ///< Create box collider
    CreateSphere,   ///< Create sphere collider
    CreateCapsule,  ///< Create capsule collider
    CreateConstraint  ///< Create constraint (link entities)
};

/**
 * @brief Editor settings
 */
struct EditorSettings {
    /// Gizmos
    bool showGizmos = true;              ///< Show gizmos
    float gizmoSize = 1.0f;              ///< Gizmo size
    float gizmoAlpha = 0.8f;             ///< Gizmo transparency
    
    /// Snap
    bool snapToGrid = false;             ///< Snap to grid
    float gridSize = 1.0f;               ///< Grid size (meters)
    bool snapToSurface = false;          ///< Snap to surface (raycast)
    
    /// Preview
    bool previewPhysics = false;         ///< Simulate physics in editor
    float previewSpeed = 1.0f;           ///< Preview speed multiplier
    
    /// Selection
    glm::vec4 selectionColor = glm::vec4(1, 0.5f, 0, 1);  ///< Orange
    
    /**
     * @brief Create default settings
     * @return EditorSettings
     */
    static EditorSettings Default() {
        return {};
    }
};

/**
 * @brief Physics editor tools
 * 
 * Interactive tools for editing physics objects in editor.
 * 
 * **Features:**
 * - Interactive gizmos (move, rotate, scale colliders)
 * - Property inspectors (edit component properties in UI)
 * - Creation tools (box, sphere, capsule colliders)
 * - Constraint tools (connect entities with constraints)
 * - Real-time preview (simulate physics in editor)
 * - Undo/Redo support
 * - Snap to grid/surface
 * 
 * @example
 * @code
 * // Enable editor tools
 * Physics::Resource::PhysicsEditorTools::Enable(core);
 * 
 * // Set current tool (Move)
 * Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Move);
 * 
 * // Select entity
 * Physics::Resource::PhysicsEditorTools::SelectEntity(core, entity);
 * 
 * // Update in editor loop
 * Physics::Resource::PhysicsEditorTools::Update(core, deltaTime);
 * 
 * // Render gizmos
 * Physics::Resource::PhysicsEditorTools::RenderGizmos(core);
 * @endcode
 */
class PhysicsEditorTools {
public:
    /**
     * @brief Enable editor tools
     * @param core Engine core
     * @param settings Editor settings
     */
    static void Enable(Engine::Core &core, const EditorSettings &settings = EditorSettings::Default());
    
    /**
     * @brief Disable editor tools
     * @param core Engine core
     */
    static void Disable(Engine::Core &core);
    
    /**
     * @brief Check if editor tools are enabled
     * @param core Engine core
     * @return true if enabled
     */
    static bool IsEnabled(Engine::Core &core);
    
    /**
     * @brief Update editor tools (call every frame)
     * @param core Engine core
     * @param deltaTime Delta time (seconds)
     */
    static void Update(Engine::Core &core, float deltaTime);
    
    /**
     * @brief Render gizmos (call in render loop)
     * @param core Engine core
     */
    static void RenderGizmos(Engine::Core &core);
    
    /**
     * @brief Set current tool
     * @param core Engine core
     * @param tool Tool to activate
     */
    static void SetTool(Engine::Core &core, EditorTool tool);
    
    /**
     * @brief Get current tool
     * @param core Engine core
     * @return Current tool
     */
    static EditorTool GetTool(Engine::Core &core);
    
    /**
     * @brief Select entity
     * @param core Engine core
     * @param entity Entity to select
     */
    static void SelectEntity(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Deselect entity
     * @param core Engine core
     */
    static void DeselectEntity(Engine::Core &core);
    
    /**
     * @brief Get selected entity
     * @param core Engine core
     * @return Selected entity (or invalid if none)
     */
    static Engine::Entity GetSelectedEntity(Engine::Core &core);
    
    /**
     * @brief Raycast pick entity (mouse picking)
     * @param core Engine core
     * @param ray Ray from camera
     * @return Picked entity (or invalid if none)
     */
    static Engine::Entity PickEntity(Engine::Core &core, const glm::vec3 &rayOrigin, const glm::vec3 &rayDir);
    
    /**
     * @brief Create box collider at cursor
     * @param core Engine core
     * @param position World position
     * @param extents Box extents
     * @return Created entity
     */
    static Engine::Entity CreateBoxCollider(Engine::Core &core, const glm::vec3 &position, const glm::vec3 &extents);
    
    /**
     * @brief Create sphere collider at cursor
     * @param core Engine core
     * @param position World position
     * @param radius Sphere radius
     * @return Created entity
     */
    static Engine::Entity CreateSphereCollider(Engine::Core &core, const glm::vec3 &position, float radius);
    
    /**
     * @brief Create capsule collider at cursor
     * @param core Engine core
     * @param position World position
     * @param radius Capsule radius
     * @param height Capsule height
     * @return Created entity
     */
    static Engine::Entity CreateCapsuleCollider(Engine::Core &core, const glm::vec3 &position, float radius, float height);
    
    /**
     * @brief Create distance constraint between two entities
     * @param core Engine core
     * @param entityA First entity
     * @param entityB Second entity
     * @return Constraint entity
     */
    static Engine::Entity CreateDistanceConstraint(Engine::Core &core, Engine::Entity entityA, Engine::Entity entityB);
    
    /**
     * @brief Start physics preview (simulate in editor)
     * @param core Engine core
     */
    static void StartPreview(Engine::Core &core);
    
    /**
     * @brief Stop physics preview
     * @param core Engine core
     * @param restore true to restore original state
     */
    static void StopPreview(Engine::Core &core, bool restore = true);
    
    /**
     * @brief Check if preview is active
     * @param core Engine core
     * @return true if preview is active
     */
    static bool IsPreviewActive(Engine::Core &core);
    
    /**
     * @brief Undo last operation
     * @param core Engine core
     */
    static void Undo(Engine::Core &core);
    
    /**
     * @brief Redo last undone operation
     * @param core Engine core
     */
    static void Redo(Engine::Core &core);
    
    /**
     * @brief Check if undo is available
     * @param core Engine core
     * @return true if undo stack is not empty
     */
    static bool CanUndo(Engine::Core &core);
    
    /**
     * @brief Check if redo is available
     * @param core Engine core
     * @return true if redo stack is not empty
     */
    static bool CanRedo(Engine::Core &core);
    
    /**
     * @brief Get settings
     * @param core Engine core
     * @return Current settings
     */
    static EditorSettings GetSettings(Engine::Core &core);
    
    /**
     * @brief Set settings
     * @param core Engine core
     * @param settings New settings
     */
    static void SetSettings(Engine::Core &core, const EditorSettings &settings);
    
    /**
     * @brief Snap position to grid
     * @param position World position
     * @param gridSize Grid size
     * @return Snapped position
     */
    static glm::vec3 SnapToGrid(const glm::vec3 &position, float gridSize);
    
    /**
     * @brief Snap position to surface (raycast down)
     * @param core Engine core
     * @param position World position
     * @return Snapped position (or original if no surface)
     */
    static glm::vec3 SnapToSurface(Engine::Core &core, const glm::vec3 &position);
    
private:
    /// Internal state
    struct State {
        bool enabled = false;
        EditorSettings settings;
        EditorTool currentTool = EditorTool::None;
        Engine::Entity selectedEntity;
        bool previewActive = false;
        std::vector<nlohmann::json> undoStack;
        std::vector<nlohmann::json> redoStack;
    };
    
    static State& GetState(Engine::Core &core);
    
    /// Gizmo rendering
    static void RenderMoveGizmo(Engine::Core &core, Engine::Entity entity);
    static void RenderRotateGizmo(Engine::Core &core, Engine::Entity entity);
    static void RenderScaleGizmo(Engine::Core &core, Engine::Entity entity);
    static void RenderSelectionOutline(Engine::Core &core, Engine::Entity entity);
    
    /// Manipulation
    static void HandleMoveGizmo(Engine::Core &core, Engine::Entity entity, float deltaTime);
    static void HandleRotateGizmo(Engine::Core &core, Engine::Entity entity, float deltaTime);
    static void HandleScaleGizmo(Engine::Core &core, Engine::Entity entity, float deltaTime);
    
    /// Undo/Redo
    static void PushUndo(Engine::Core &core, const nlohmann::json &state);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create PhysicsEditorTools (8-10 hours)

- [ ] Create `resource/PhysicsEditorTools.hpp`
- [ ] Implement EditorTool enum
- [ ] Implement EditorSettings struct
- [ ] Implement Enable/Disable
- [ ] Implement SetTool/GetTool
- [ ] Implement SelectEntity/DeselectEntity
- [ ] Implement PickEntity (raycast)
- [ ] Add Doxygen comments

### Phase 2: Implement Gizmos (12-14 hours)

- [ ] Implement RenderMoveGizmo()
  - 3 colored arrows (RGB = XYZ)
  - Interactive (hover, click, drag)
- [ ] Implement RenderRotateGizmo()
  - 3 colored circles (RGB = XYZ)
  - Interactive
- [ ] Implement RenderScaleGizmo()
  - 3 colored cubes (RGB = XYZ)
  - Interactive
- [ ] Implement RenderSelectionOutline()
  - Highlight selected entity
- [ ] Integrate with Graphic plugin (line/mesh rendering)

### Phase 3: Implement Manipulation (10-12 hours)

- [ ] Implement HandleMoveGizmo()
  - Drag arrow to translate
  - Apply to collider offset
- [ ] Implement HandleRotateGizmo()
  - Drag circle to rotate
  - Apply to collider rotation
- [ ] Implement HandleScaleGizmo()
  - Drag cube to scale
  - Apply to collider size
- [ ] Snap to grid/surface

### Phase 4: Implement Creation Tools (6-8 hours)

- [ ] Implement CreateBoxCollider()
- [ ] Implement CreateSphereCollider()
- [ ] Implement CreateCapsuleCollider()
- [ ] Implement CreateDistanceConstraint()
  - Select first entity
  - Select second entity
  - Create constraint

### Phase 5: Implement Preview System (6-8 hours)

- [ ] Implement StartPreview()
  - Save scene state
  - Enable physics simulation
- [ ] Implement StopPreview()
  - Restore scene state (optional)
- [ ] Implement preview update loop

### Phase 6: Implement Undo/Redo (4-5 hours)

- [ ] Implement PushUndo()
- [ ] Implement Undo()
  - Restore previous state
- [ ] Implement Redo()
  - Restore next state
- [ ] Serialize state (JSON)

### Phase 7: Property Inspector (8-10 hours)

- [ ] Create ImGui inspector window
- [ ] Show RigidBody properties
  - Type (Dynamic/Kinematic/Static)
  - Mass, inertia
  - Damping
- [ ] Show Collider properties
  - Type (Box/Sphere/Capsule/etc.)
  - Size/radius/height
  - Offset, rotation
  - Material (friction, restitution)
- [ ] Show Constraint properties
  - Type
  - Connected entities
  - Parameters
- [ ] Editable fields (drag, slider, input)

### Phase 8: Unit Tests (6-8 hours)

- [ ] Create `tests/PhysicsEditorTest.cpp`
- [ ] Test Enable/Disable
- [ ] Test tool switching
- [ ] Test entity selection
- [ ] Test creation tools
- [ ] Test undo/redo
- [ ] Test snap functions

### Phase 9: Integration Examples (8-10 hours)

- [ ] Create editor example project
- [ ] Add toolbar (tool buttons)
- [ ] Add property inspector panel
- [ ] Add preview controls
- [ ] Add keyboard shortcuts

### Phase 10: Documentation (5-6 hours)

- [ ] Create `PHYSICS_EDITOR_GUIDE.md`
  - Explain editor tools
  - Explain gizmos
  - Explain shortcuts
- [ ] Add tutorial videos (optional)

## 🧪 Testing Checklist

### Unit Tests

- [ ] Editor tools enable/disable correctly
- [ ] Tool switching works
- [ ] Entity selection works
- [ ] Creation tools work
- [ ] Undo/redo works
- [ ] Snap functions work

### Integration Tests

- [ ] Gizmos render correctly
- [ ] Gizmos interactive
- [ ] Creation tools create entities
- [ ] Preview simulates physics
- [ ] Property inspector edits components

### User Testing

- [ ] Intuitive tool selection
- [ ] Gizmos easy to use
- [ ] Preview helpful for tuning
- [ ] Undo/redo reliable

### Edge Cases

- [ ] No entity selected (gizmos hidden)
- [ ] Select non-physics entity (no gizmos)
- [ ] Undo/redo empty stack
- [ ] Preview with no physics entities

## 📚 Code Examples

### Example 1: Basic Editor Setup

```cpp
// Enable editor tools
auto settings = Physics::Resource::EditorSettings::Default();
settings.snapToGrid = true;
settings.gridSize = 0.5f;
Physics::Resource::PhysicsEditorTools::Enable(core, settings);

// Editor loop
while (editorRunning) {
    // Update tools
    Physics::Resource::PhysicsEditorTools::Update(core, deltaTime);
    
    // Render scene
    Render(core);
    
    // Render gizmos on top
    Physics::Resource::PhysicsEditorTools::RenderGizmos(core);
    
    // Render UI
    RenderEditorUI(core);
}
```

### Example 2: Tool Toolbar (ImGui)

```cpp
void RenderToolbar(Engine::Core &core) {
    ImGui::Begin("Tools");
    
    auto currentTool = Physics::Resource::PhysicsEditorTools::GetTool(core);
    
    // Select tool
    if (ImGui::RadioButton("Select", currentTool == Physics::Resource::EditorTool::None)) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::None);
    }
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Move", currentTool == Physics::Resource::EditorTool::Move)) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Move);
    }
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Rotate", currentTool == Physics::Resource::EditorTool::Rotate)) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Rotate);
    }
    ImGui::SameLine();
    
    if (ImGui::RadioButton("Scale", currentTool == Physics::Resource::EditorTool::Scale)) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Scale);
    }
    
    ImGui::Separator();
    ImGui::Text("Create");
    
    if (ImGui::Button("Box")) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::CreateBox);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Sphere")) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::CreateSphere);
    }
    ImGui::SameLine();
    
    if (ImGui::Button("Capsule")) {
        Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::CreateCapsule);
    }
    
    ImGui::End();
}
```

### Example 3: Property Inspector

```cpp
void RenderPropertyInspector(Engine::Core &core) {
    ImGui::Begin("Inspector");
    
    auto selected = Physics::Resource::PhysicsEditorTools::GetSelectedEntity(core);
    if (!selected.IsValid(core)) {
        ImGui::Text("No entity selected");
        ImGui::End();
        return;
    }
    
    // RigidBody properties
    if (selected.HasComponent<Physics::Component::RigidBody>(core)) {
        if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto &rb = selected.GetComponent<Physics::Component::RigidBody>(core);
            
            // Type
            const char* types[] = { "Dynamic", "Kinematic", "Static" };
            int typeIndex = static_cast<int>(rb.type);
            if (ImGui::Combo("Type", &typeIndex, types, 3)) {
                rb.type = static_cast<Physics::Component::RigidBodyType>(typeIndex);
                // TODO: Push undo
            }
            
            // Mass (only for dynamic)
            if (rb.type == Physics::Component::RigidBodyType::Dynamic) {
                if (ImGui::DragFloat("Mass", &rb.mass, 0.1f, 0.1f, 1000.0f)) {
                    // TODO: Push undo
                }
            }
            
            // Damping
            if (ImGui::DragFloat("Linear Damping", &rb.linearDamping, 0.01f, 0.0f, 1.0f)) {
                // TODO: Push undo
            }
            
            if (ImGui::DragFloat("Angular Damping", &rb.angularDamping, 0.01f, 0.0f, 1.0f)) {
                // TODO: Push undo
            }
        }
    }
    
    // Collider properties
    if (selected.HasComponent<Physics::Component::BoxCollider>(core)) {
        if (ImGui::CollapsingHeader("Box Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
            auto &collider = selected.GetComponent<Physics::Component::BoxCollider>(core);
            
            if (ImGui::DragFloat3("Half Extents", &collider.halfExtents.x, 0.1f, 0.1f, 100.0f)) {
                // TODO: Push undo
            }
            
            if (ImGui::DragFloat3("Offset", &collider.offset.x, 0.1f)) {
                // TODO: Push undo
            }
        }
    }
    
    ImGui::End();
}
```

### Example 4: Mouse Picking

```cpp
// Mouse click to select entity
if (Input::IsMouseButtonPressed(Input::MouseButton::Left)) {
    // Get mouse ray from camera
    auto ray = Camera::GetMouseRay(core);
    
    // Pick entity
    auto picked = Physics::Resource::PhysicsEditorTools::PickEntity(
        core, ray.origin, ray.direction
    );
    
    if (picked.IsValid(core)) {
        // Select entity
        Physics::Resource::PhysicsEditorTools::SelectEntity(core, picked);
        Logger::Info("Selected entity {}", entt::to_integral(picked));
    } else {
        // Deselect
        Physics::Resource::PhysicsEditorTools::DeselectEntity(core);
    }
}
```

### Example 5: Keyboard Shortcuts

```cpp
// Keyboard shortcuts
if (Input::IsKeyPressed(Input::Key::W)) {
    // Move tool
    Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Move);
}

if (Input::IsKeyPressed(Input::Key::E)) {
    // Rotate tool
    Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Rotate);
}

if (Input::IsKeyPressed(Input::Key::R)) {
    // Scale tool
    Physics::Resource::PhysicsEditorTools::SetTool(core, Physics::Resource::EditorTool::Scale);
}

if (Input::IsKeyPressed(Input::Key::Delete)) {
    // Delete selected entity
    auto selected = Physics::Resource::PhysicsEditorTools::GetSelectedEntity(core);
    if (selected.IsValid(core)) {
        // TODO: Push undo before deleting
        core.DestroyEntity(selected);
        Physics::Resource::PhysicsEditorTools::DeselectEntity(core);
    }
}

if (Input::IsKeyDown(Input::Key::LeftControl) && Input::IsKeyPressed(Input::Key::Z)) {
    // Undo
    if (Physics::Resource::PhysicsEditorTools::CanUndo(core)) {
        Physics::Resource::PhysicsEditorTools::Undo(core);
    }
}

if (Input::IsKeyDown(Input::Key::LeftControl) && Input::IsKeyPressed(Input::Key::Y)) {
    // Redo
    if (Physics::Resource::PhysicsEditorTools::CanRedo(core)) {
        Physics::Resource::PhysicsEditorTools::Redo(core);
    }
}
```

### Example 6: Physics Preview

```cpp
// Preview controls
void RenderPreviewControls(Engine::Core &core) {
    ImGui::Begin("Preview");
    
    bool isPreviewActive = Physics::Resource::PhysicsEditorTools::IsPreviewActive(core);
    
    if (isPreviewActive) {
        if (ImGui::Button("Stop Preview")) {
            Physics::Resource::PhysicsEditorTools::StopPreview(core, true);  // Restore
        }
        ImGui::SameLine();
        ImGui::Text("(Simulating...)");
    } else {
        if (ImGui::Button("Start Preview")) {
            Physics::Resource::PhysicsEditorTools::StartPreview(core);
        }
    }
    
    ImGui::Separator();
    
    auto settings = Physics::Resource::PhysicsEditorTools::GetSettings(core);
    if (ImGui::SliderFloat("Speed", &settings.previewSpeed, 0.1f, 2.0f)) {
        Physics::Resource::PhysicsEditorTools::SetSettings(core, settings);
    }
    
    ImGui::End();
}
```

## 🐛 Error Handling

### Cases to Handle

1. **No entity selected**
   ```cpp
   if (!selectedEntity.IsValid(core)) {
       // Don't render gizmos
       return;
   }
   ```

2. **Invalid tool**
   ```cpp
   if (tool >= EditorTool::COUNT) {
       Logger::Error("PhysicsEditorTools: Invalid tool {}", static_cast<int>(tool));
       return;
   }
   ```

3. **Undo/Redo stack empty**
   ```cpp
   if (undoStack.empty()) {
       Logger::Warning("PhysicsEditorTools: Nothing to undo");
       return;
   }
   ```

## 💡 Design Considerations

### Gizmo Rendering

- Use Graphic plugin line/mesh renderer
- Render in screen space (constant size)
- Highlight on hover (change color)
- Interactive (raycast picking)

### Undo/Redo

- Serialize state before modification
- Push to undo stack
- Limit stack size (e.g., 50 operations)
- Clear redo stack on new operation

## 📊 Success Criteria

- [ ] PhysicsEditorTools resource implemented
- [ ] Gizmos render correctly (move, rotate, scale)
- [ ] Gizmos interactive
- [ ] Selection works
- [ ] Creation tools work
- [ ] Property inspector works
- [ ] Preview works
- [ ] Undo/redo works
- [ ] Unit tests pass
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Unity Editor: https://docs.unity3d.com/Manual/UsingTheEditor.html
- Unreal Editor: https://docs.unrealengine.com/en-US/Editor/
- ImGuizmo: https://github.com/CedricGuillemet/ImGuizmo

## 🚀 Follow-up Issues

- #028: Scene System (integrate with scene management)
- #029: Asset Pipeline (import/export physics assets)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.8`, `priority:medium`, `editor`, `tools`, `ui`  
**Milestone:** v0.8 - Ecosystem & Tools
