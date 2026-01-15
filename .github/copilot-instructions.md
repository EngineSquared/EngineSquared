# Engine² Copilot Instructions

> **Complete Architecture Reference**: See [docs/ANALYSIS.md](../docs/ANALYSIS.md) for detailed data flow diagrams and implementation patterns.

## Architecture Overview

Engine² is an ECS-based C++20 game engine using **entt** for entity management and **Jolt Physics** for simulation and **WebGPU** for rendering.

### Core Components
- **`Engine::Core`** - Central registry managing entities, resources, schedulers, and plugins
- **`Engine::Entity`** - Wrapper around `entt::entity` (32-bit ID with version bits) with helper methods
- **Plugins** - Modular features extending `Engine::APlugin` (Physics, Graphic, Input, etc.)
- **Schedulers** - Control system execution timing: `Startup`, `Update`, `FixedTimeUpdate`, `RelativeTimeUpdate`, `Shutdown`

### Plugin System Pattern
Plugins follow a consistent structure in `src/plugin/<name>/`:
```
src/plugin/<name>/
├── xmake.lua           # Build config with dependencies
├── src/
│   ├── <Name>.hpp      # Public header aggregating all exports
│   ├── <Name>.pch.hpp  # Precompiled header
│   ├── plugin/Plugin<Name>.hpp  # Plugin class declaration
│   ├── component/      # ECS components (public user-facing)
│   ├── system/         # Systems (functions taking Engine::Core&)
│   ├── resource/       # Singleton resources
│   ├── event/          # Event structures (for Event plugin integration)
│   ├── exception/      # Custom exceptions
│   ├── helper/         # Public utility functions
│   └── utils/          # Internal helpers (not exposed in public API)
└── tests/              # GTest unit tests
```

### Key Patterns
- **Systems**: Functions with signature `void SystemName(Engine::Core &core)`
- **Components**: Plain structs registered via `entity.AddComponent<T>(core, ...)`
- **Resources**: Singletons accessed via `core.GetResource<T>()` / `core.RegisterResource<T>()`
- **Plugin dependencies**: Use `RequirePlugins<OtherPlugin>()` in `Bind()`
- **EnTT Hooks**: Use `registry.on_construct<T>().connect<Handler>()` for automatic component lifecycle

## Build & Test Commands

```bash
# Build
xmake build -y              # Build with auto-install dependencies
xmake build -m debug        # Debug build

# Run tests
xmake test -y               # Run all unit tests

# Format code
find . -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
```

## Code Conventions

### Naming
- Namespaces: `Plugin::Component`, `Plugin::System`, `Plugin::Resource`
- Files match class names: `RigidBody.hpp` → `Physics::Component::RigidBody`
- Tests: `*Test.cpp` using GTest with `TEST(GroupName, TestName)`

### Component Example
```cpp
// In component/MyComponent.hpp
namespace MyPlugin::Component {
struct MyComponent {
    float value = 0.0f;
    // Default/move/copy constructors typically defaulted
};
}
```

### System Registration
```cpp
// In plugin/MyPlugin.cpp
void MyPlugin::Plugin::Bind() {
    RequirePlugins<Event::Plugin>();  // Declare dependencies first

    RegisterSystems<Engine::Scheduler::Startup>(System::InitMySystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::UpdateMySystem);
}
```

### Physics-Specific Patterns

#### Public + Internal Component Pattern
Physics uses a two-component pattern for Jolt integration:
- **Public**: User-facing settings (e.g., `RigidBody`, `SoftBody`)
- **Internal**: Jolt runtime data (e.g., `RigidBodyInternal` with `JPH::BodyID`)

```cpp
// User adds public component → hook creates internal automatically
entity.AddComponent<Physics::Component::RigidBody>(core, RigidBody::CreateDynamic(5.0f));
// System hook automatically creates RigidBodyInternal with Jolt BodyID
```

#### EnTT Hook Registration
```cpp
void InitMySystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    registry.ctx().emplace<Engine::Core *>(&core);  // Store core for hook access

    registry.on_construct<Component::MyComponent>().connect<&OnMyComponentConstruct>();
    registry.on_destroy<Component::MyComponent>().connect<&OnMyComponentDestroy>();
}
```

#### Type Conversions (GLM ↔ Jolt)
```cpp
#include "utils/JoltConversions.hpp"
// GLM → Jolt
JPH::Vec3 jv = Physics::Utils::ToJoltVec3(glmVec);
JPH::Quat jq = Physics::Utils::ToJoltQuat(glmQuat);
// Jolt → GLM
glm::vec3 gv = Physics::Utils::FromJoltVec3(joltVec);
glm::quat gq = Physics::Utils::FromJoltQuat(joltQuat);
```

#### Thread Safety in Contact Callbacks
Jolt calls ContactListener from worker threads. Buffer events and flush on main thread:
```cpp
// In ContactListenerImpl - buffer events under mutex
void OnContactAdded(...) {
    std::scoped_lock lock(_bufferMutex);
    _bufferedAdded.emplace_back(event);
}

// Called from PhysicsUpdate system (main thread)
void ProcessBufferedEvents(Core &core) {
    std::vector<Event> local;
    { std::scoped_lock lock(_bufferMutex); local.swap(_buffered); }
    for (auto &e : local) eventManager.PushEvent(e);
}
```

## Event System

Register callbacks with scheduler association:
```cpp
auto &em = core.GetResource<Event::Resource::EventManager>();
// Process during FixedTimeUpdate
em.RegisterCallback<MyEvent, Engine::Scheduler::FixedTimeUpdate>(
    [](const MyEvent &e) { /* handler */ });
// Or immediate execution (no scheduler)
em.RegisterCallback<MyEvent>([](const MyEvent &e) { /* immediate */ });
```

## Git Workflow

### Branches
- Generated from GitHub issues, or manually with lowercase and dashes: `feat-soft-body-system`
- Common prefixes: `feat-`, `fix-`, `refactor-`, `docs-`

### Commits (Angular Convention)
```
<type>(<scope>): <subject>

feat(physics): add SoftBody component
fix(graphic): resolve texture loading crash
refactor(core): simplify scheduler execution
docs(readme): update build instructions
```
Types: `feat`, `fix`, `refactor`, `docs`, `test`, `chore`, `perf`

## Scheduler Execution Order

Schedulers use topological sort with declared dependencies. Default order:
`Startup` → `Update` → `FixedTimeUpdate` / `RelativeTimeUpdate` → `Shutdown`

Configure with:
```cpp
core.SetSchedulerBefore<Scheduler::Update, Scheduler::FixedTimeUpdate>();
core.SetSchedulerAfter<Scheduler::Shutdown, Scheduler::Update>();
```

## Examples & Integration Tests
The `examples/` folder contains integration tests and implementation documentation:
- `basic_core_usage/` - Minimal engine setup
- `physics_usage/` - RigidBody and collision examples
- `softbody_usage/` - Soft body physics demo (OBJ loading, pinning vertices)
- `graphic_usage/` - Rendering pipeline examples
- `graphic_usage_with_physics/` - Combined graphics + physics

Use these as reference implementations when creating new features.

## Code Quality Standards

### Error Handling
- Create **custom exceptions** per module in `exception/` folder
- Inherit from `std::exception` with descriptive names: `RigidBodyError`, `OBJLoaderError`
- Never use raw `std::runtime_error` directly

```cpp
// In exception/MyModuleError.hpp
namespace MyPlugin::Exception {
class MyModuleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};
}
```

### Coding Principles (Enforced by CodeRabbit AI)
- **Clean Code**: Meaningful names, small focused functions, single responsibility
- **Guard Clauses**: Early returns to reduce nesting
- **SOLID**: Single responsibility, Open/closed, Liskov substitution, Interface segregation, Dependency inversion
- **Modern C++20**: `std::span`, `std::optional`, concepts, ranges, structured bindings
- **Doxygen**: Document all public APIs with `@brief`, `@param`, `@return`, `@note`

```cpp
// Guard clause example
void ProcessEntity(Engine::Core &core, Engine::Entity entity) {
    if (!entity.IsValid())
        return;

    if (!entity.HasComponents<Component::Transform>(core))
        return;

    // Main logic here...
}
```

### PR Review
CodeRabbit AI bot reviews all PRs with strict quality checks. To avoid issues:
- No magic numbers (use named constants)
- No commented-out code
- Proper const-correctness
- RAII for resource management
- Complete error handling paths

## Dependencies (xmake packages)
- `entt v3.15.0` - ECS framework
- `joltphysics v5.4.0` - Physics engine
- `glm 1.0.1` - Math library
- `spdlog v1.16.0` - Logging (use `Log::Info()`, `Log::Debug()`, etc.)
- `gtest v1.17.0` - Unit testing
- `wgpu-native` - Graphics backend
- `glfw3webgpu` - GLFW-WebGPU bridge
- `stb`, `lodepng` - Image loading

## Graphic Plugin Patterns

### Public → GPU Component Pattern
Graphic uses the same pattern as Physics for automatic GPU resource creation:
```cpp
// User adds Object::Component::Mesh → hook creates Graphic::Component::GPUMesh
entity.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());
// System hook automatically creates:
// - GPUMesh (with buffer IDs)
// - PointGPUBuffer (vertex data)
// - IndexGPUBuffer (index data)
```

### GPU Components
| Component | Purpose | Contains |
|-----------|---------|----------|
| `GPUMesh` | Geometry | `pointBufferId`, `indexBufferId` |
| `GPUTransform` | Model matrix | `modelMatrixBuffer`, `bindGroup` |
| `GPUCamera` | View/Projection | `buffer`, `bindGroup`, `pipeline`, `targetTexture` |
| `GPUMaterial` | Material data | `buffer`, `texture`, `sampler`, `bindGroup` |

### RenderingPipeline Schedulers
The Graphic plugin uses custom schedulers from RenderingPipeline:
- `RenderingPipeline::Init` → GLFW setup (before Setup)
- `RenderingPipeline::Setup` → WebGPU initialization (before Startup)
- `RenderingPipeline::Preparation` → GPU buffer updates (after Update)
- `RenderingPipeline::CommandCreation` → Render pass execution
- `RenderingPipeline::Presentation` → Surface present

### Creating Custom Render Passes
```cpp
class MyRenderPass : public ASingleExecutionRenderPass<MyRenderPass> {
public:
    MyRenderPass() : ASingleExecutionRenderPass("MY_PASS") {}

    void UniqueRenderCallback(wgpu::RenderPassEncoder &pass, Core &core) override {
        // Custom rendering logic
    }
};
```

### Default Shader Bind Groups
| Group | Binding | Data |
|-------|---------|------|
| 0 | 0 | Camera (viewProjectionMatrix) |
| 1 | 0 | Model (modelMatrix, normalMatrix) |
| 2 | 0-2 | Material (emission, texture, sampler) |
| 3 | 0-1 | Lights (ambient, point lights array) |

### Key Graphic Constants
```cpp
// Default render pipeline
Graphic::Utils::DEFAULT_RENDER_GRAPH_ID
Graphic::Utils::DEFAULT_RENDER_PASS_SHADER_ID
Graphic::Utils::DEFAULT_MATERIAL_BIND_GROUP_ID
Graphic::Utils::LIGHTS_BIND_GROUP_ID

// Textures
System::END_RENDER_TEXTURE_ID
System::END_DEPTH_RENDER_TEXTURE_ID
```

## JoltPhysics Reference (Local Clone)

The project includes a local JoltPhysics clone at `JoltPhysics/` for reference:
- `JoltPhysics/Jolt/Physics/Body/` - Body, BodyCreationSettings
- `JoltPhysics/Jolt/Physics/SoftBody/` - SoftBodySharedSettings, SoftBodyCreationSettings
- `JoltPhysics/Jolt/Physics/Constraints/` - All constraint types
- `JoltPhysics/Samples/` - Official examples

Key Jolt patterns:
- Always include `<Jolt/Jolt.h>` first
- Use `JPH::Ref<T>` for reference-counted Jolt objects
- SoftBody requires `SoftBodySharedSettings::Optimize()` after constraint creation
- Store `entt::entity` in `BodyCreationSettings::mUserData` for collision callbacks

## File Header Template
All source files should include the standard MIT license header with `@file`, `@brief`, `@author`, `@version`, `@date` Doxygen tags.
