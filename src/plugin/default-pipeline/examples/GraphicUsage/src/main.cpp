/**************************************************************************
 * EngineSquared - Graphic Usage Example
 *
 * This example demonstrates how to use the Graphic plugin.
 **************************************************************************/

#include "Engine.hpp"

#include "DefaultPipeline.hpp"
#include "Graphic.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"

Object::Component::Mesh CreateCustomMesh(void)
{
    Object::Component::Mesh mesh;

    mesh.SetVertices({
        {-0.5f, 0.0f, -0.5f},
        {0.5f,  0.0f, -0.5f},
        {0.5f,  0.0f, 0.5f },
        {-0.5f, 0.0f, 0.5f }
    });

    mesh.SetNormals({
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    });

    mesh.SetTexCoords({
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f}
    });

    mesh.SetIndices({0, 1, 2, 2, 3, 0});

    return mesh;
}

void Setup(Engine::Core &core)
{
    auto cube = core.CreateEntity();

    cube.AddComponent<Object::Component::Transform>();
    cube.AddComponent<Object::Component::Mesh>(Object::Utils::GenerateCubeMesh());

    auto customMesh = core.CreateEntity();
    customMesh.AddComponent<Object::Component::Transform>(glm::vec3(1.5f, 0.0f, 0.0f));
    customMesh.AddComponent<Object::Component::Mesh>(CreateCustomMesh());

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.75f, 1.0f, -2.5f), glm::vec3(1.0f), glm::quat(glm::vec3(glm::radians(20.0f), 0.0f, 0.0f)));
    camera.AddComponent<Object::Component::Camera>();
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &core) {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
               WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw GraphicExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.Run();

    return 0;
}
