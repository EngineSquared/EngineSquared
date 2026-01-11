# Engine² Copilot Instructions

## Architecture Overview

Engine² is an ECS-based C++20 game engine using **entt** for entity management and **Jolt Physics** for simulation and **WebGPU** for rendering.

### Core Components
- **`Engine::Core`** - Central registry managing entities, resources, schedulers, and plugins
- **`Engine::Entity`** - Wrapper around `entt::entity` with helper methods for component management
- **Plugins** - Modular features extending `Engine::APlugin` (Physics, Graphic, Input, etc.)
- **Schedulers** - Control system execution timing: `Startup`, `Update`, `FixedTimeUpdate`, `Shutdown`

### Plugin System Pattern
Plugins follow a consistent structure in `src/plugin/<name>/`:
```
src/plugin/<name>/
├── xmake.lua           # Build config with dependencies
├── src/
│   ├── <Name>.hpp      # Public header aggregating all exports
│   ├── <Name>.pch.hpp  # Precompiled header
│   ├── plugin/Plugin<Name>.hpp  # Plugin class declaration
│   ├── component/      # ECS components
│   ├── system/         # Systems (functions taking Engine::Core&)
│   ├── resource/       # Singleton resources
│   ├── exception/      # Custom exceptions
│   ├── helper/         # Public utility functions
│   └── utils/          # Internal helpers
└── tests/              # GTest unit tests
```

### Key Patterns
- **Systems**: Functions with signature `void SystemName(Engine::Core &core)`
- **Components**: Plain structs registered via `entity.AddComponent<T>(core, ...)`
- **Resources**: Singletons accessed via `core.GetResource<T>()` / `core.RegisterResource<T>()`
- **Plugin dependencies**: Use `RequirePlugins<OtherPlugin>()` in `Bind()`

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

### Physics-Specific
- Use `Physics::Utils::ToJoltVec3()` / `FromJoltVec3()` for GLM↔Jolt conversions
- RigidBody hooks: Systems registered via `registry.on_construct<Component>().connect<Handler>`
- Collision events buffered and processed via `ContactListenerImpl::ProcessBufferedEvents()`

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
Fixed order (not configurable): `Startup` → `Update` → `FixedTimeUpdate` → `Shutdown`

## Examples & Integration Tests
The `examples/` folder contains integration tests and implementation documentation:
- `basic_core_usage/` - Minimal engine setup
- `physics_usage/` - RigidBody and collision examples
- `softbody_usage/` - Soft body physics demo
- `graphic_usage/` - Rendering pipeline examples

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

## File Header Template
All source files should include the standard MIT license header with `@file`, `@brief`, `@author`, `@version`, `@date` Doxygen tags.
