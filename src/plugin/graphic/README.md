# Graphic Plugin

Hardware-accelerated graphics backend for [EngineSquared](https://github.com/EngineSquared/EngineSquared), built on **WebGPU** via wgpu-native. It provides GPU context management, shaders, textures, buffers, bind groups, and a render-graph pipeline integrated with the engine’s rendering stages.

---

## Description

The Graphic plugin adds a full graphics stack to EngineSquared:

- **WebGPU / wgpu-native** — Cross-platform GPU API (Vulkan, Metal, D3D12).
- **Context & device** — Instance, adapter, device, and queue setup with configurable limits and features.
- **Resources** — Shaders (WGSL), textures, samplers, GPU buffers, and bind groups.
- **Render graph** — Declarative render passes with dependencies and topological execution.
- **Pipeline integration** — Hooks into the engine’s RenderingPipeline (Setup, Preparation, CommandCreation, Presentation) and Shutdown.

It registers resources (e.g. `Context`, `GraphicSettings`, `ShaderContainer`, `TextureContainer`, `GPUBufferContainer`, `SamplerContainer`, `BindGroupManager`, `RenderGraphContainer`) and systems for initialization, frame preparation, command recording, presentation, and cleanup. Other plugins (e.g. Default Pipeline, RmlUi) can depend on it for rendering.

---

## Dependencies

### External (xmake packages)

| Package        | Purpose                    |
|----------------|----------------------------|
| `wgpu-native`  | WebGPU implementation      |
| `glfw`         | Windowing (optional)       |
| `glfw3webgpu`  | GLFW–WebGPU surface        |
| `glm`          | Math types                 |
| `entt`         | ECS / hashed names         |
| `spdlog`       | Logging                    |
| `fmt`          | Formatting                 |
| `stb`          | Image loading              |
| `lodepng`      | PNG support                |
| `tinyobjloader`| OBJ mesh loading           |

### Engine

- **EngineSquaredCore** — Core engine and scheduler
- **PluginObject** — Entities and components (e.g. Mesh, Transform)
- **PluginRenderingPipeline** — Rendering stages (Setup, Preparation, CommandCreation, Presentation)
- **PluginWindow** — Window and surface (when using a window)
- **PluginEvent** — Event handling
- **UtilsLog** — Logging utilities

The plugin **requires** `RenderingPipeline::Plugin` and `Event::Plugin`; use with `Window::Plugin` when rendering to a window.

---

## Install

The Graphic plugin is built as part of EngineSquared with [xmake](https://xmake.io/). There is no separate install step.

1. **Clone and enter the repo**
   ```bash
   git clone https://github.com/EngineSquared/EngineSquared.git
   cd EngineSquared
   ```

2. **Install xmake** (if needed)  
   See [xmake install](https://xmake.io/#/getting_started/installation).

3. **Configure and build**
   ```bash
   xmake config
   xmake build
   ```

   The static target `PluginGraphic` is built with the rest of the engine. Link it (or the main `EngineSquared` target that already depends on it) into your application.

4. **Run tests** (optional)
   ```bash
   xmake build PluginGraphicTests
   xmake build RenderGraphTest   # or any test binary from tests/
   ```

---

## Configuration

Configure the plugin by editing the **GraphicSettings** resource **before** running the rendering pipeline (e.g. in a system registered for `RenderingPipeline::Init` or earlier). Access it via the core:

```cpp
#include "resource/GraphicSettings.hpp"

auto& settings = core.GetResource<Graphic::Resource::GraphicSettings>();
```

### Window system

Choose how the surface is created (e.g. for headless tests vs windowed apps):

```cpp
// Use a window (default; requires PluginWindow)
settings.SetWindowSystem(Graphic::Resource::WindowSystem::GLFW);

// No window (headless; no surface from window)
settings.SetWindowSystem(Graphic::Resource::WindowSystem::None);
```

### Power preference

```cpp
settings.SetPowerPreference(Graphic::Resource::PowerPreference::HighPerformance);
// or
settings.SetPowerPreference(Graphic::Resource::PowerPreference::LowPower);
```

### Device limits and features

```cpp
// Optional: adjust limits (see resource/Limits.hpp)
settings.GetWantedLimits() = Limits(wgpu::Default);  // or custom limits

// Require specific WebGPU features
settings.AddRequiredFeature(wgpu::FeatureName::TextureCompressionBC);
```

### Uncaptured device errors

Set a custom callback for WebGPU uncaptured errors (e.g. to log and/or throw in debug):

```cpp
settings.SetOnErrorCallback(
    [](WGPUDevice const*, WGPUErrorType type, WGPUStringView message,
       void*, void*) {
        Log::Error(fmt::format("WebGPU error: type {:x} ({})",
            static_cast<uint32_t>(type),
            std::string(message.data, message.length)));
        // optionally: throw or abort
    });
```

---

## Usage

### Adding the plugin

Ensure required plugins are added; then add the Graphic plugin:

```cpp
#include "Engine.hpp"
#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"

int main() {
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, Graphic::Plugin>();
    // Configure GraphicSettings in Init if needed (see Configuration)
    core.RunCore();
    return 0;
}
```

Other plugins that depend on graphics (e.g. Default Pipeline, RmlUi) will typically `RequirePlugins<..., Graphic::Plugin>()`; your app only needs to add the plugin (and Window, if you render to a window).

### Configuring settings at startup

```cpp
core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core& core) {
    auto& settings = core.GetResource<Graphic::Resource::GraphicSettings>();
    settings.SetWindowSystem(Graphic::Resource::WindowSystem::GLFW);
    settings.SetPowerPreference(Graphic::Resource::PowerPreference::HighPerformance);
    settings.SetOnErrorCallback([](WGPUDevice const*, WGPUErrorType type,
                                   WGPUStringView message, void*, void*) {
        Log::Error(fmt::format("Device error: {}", std::string(message.data, message.length)));
    });
});
```

### Headless (no window)

Useful for tests or offscreen rendering:

```cpp
core.AddPlugins<Graphic::Plugin>();
core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core& core) {
    core.GetResource<Graphic::Resource::GraphicSettings>()
        .SetWindowSystem(Graphic::Resource::WindowSystem::None);
});
core.RunSystems();
```

### Accessing graphics resources

After the pipeline has run setup, you can use the registered resources:

```cpp
auto& context    = core.GetResource<Graphic::Resource::Context>();
auto& shaders    = core.GetResource<Graphic::Resource::ShaderContainer>();
auto& textures   = core.GetResource<Graphic::Resource::TextureContainer>();
auto& buffers    = core.GetResource<Graphic::Resource::GPUBufferContainer>();
auto& samplers   = core.GetResource<Graphic::Resource::SamplerContainer>();
auto& bindGroups = core.GetResource<Graphic::Resource::BindGroupManager>();
auto& renderGraph= core.GetResource<Graphic::Resource::RenderGraphContainer>();
```

### Shader from WGSL source

```cpp
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"
#include "resource/ShaderContainer.hpp"

Graphic::Resource::ShaderDescriptor desc;
desc.setShader(R"(
    @vertex fn vs_main() -> @builtin(position) vec4f {
        return vec4f(0.0, 0.0, 0.0, 1.0);
    }
    @fragment fn fs_main() -> @location(0) vec4f {
        return vec4f(1.0, 0.0, 0.0, 1.0);
    }
)")
  .setVertexEntryPoint("vs_main")
  .setFragmentEntryPoint("fs_main")
  .addColorTargetState(Graphic::Resource::ColorTargetState(wgpu::TextureFormat::BGRA8Unorm));

auto& context = core.GetResource<Graphic::Resource::Context>();
auto& container = core.GetResource<Graphic::Resource::ShaderContainer>();
Graphic::Resource::Shader shader = Graphic::Resource::Shader::Create(desc, context);
container.Add("my_shader", std::move(shader));
```

### Render graph with a render pass

```cpp
#include "resource/RenderGraph.hpp"
#include "resource/RenderGraphContainer.hpp"

auto& graphContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();
auto& graph = graphContainer.Get();

graph.Add<MyRenderPass>("main_pass", MyRenderPass(core, ...));
graph.SetDependency("other_pass", "main_pass");
// Execution order is resolved automatically; then each frame:
// graph.Execute(core) is driven by the pipeline's CommandCreation stage
```

(Your render pass type must satisfy the plugin’s `ARenderPass` interface and be registered/used within the engine’s render graph execution.)

### Full example (minimal app with error callback)

See `examples/graphic_usage/src/main.cpp` for a complete sample: plugin setup, `GraphicSettings` (including `SetOnErrorCallback`), and a simple scene with mesh and camera.

---

## License

Copyright (c) 2025 EngineSquared.  
This plugin is part of EngineSquared and is distributed under the **MIT License**.  
See the [LICENSE](https://github.com/EngineSquared/EngineSquared/blob/main/LICENSE) file in the repository root for the full text.
