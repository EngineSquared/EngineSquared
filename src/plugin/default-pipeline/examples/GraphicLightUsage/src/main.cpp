#include "Engine.hpp"

#include "DefaultPipeline.hpp"
#include "Graphic.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"

void Setup(Engine::Core &core)
{
    auto cube = core.CreateEntity();
    cube.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));
    cube.AddComponent<Object::Component::Mesh>(Object::Utils::GenerateCubeMesh());

    auto plane = core.CreateEntity();
    plane.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, -1.0f, 0.0f));
    plane.AddComponent<Object::Component::Mesh>(Object::Utils::GeneratePlaneMesh(5.0f, 5.0f, 10u, 10u));

    auto camera = core.CreateEntity();
    camera.AddComponent<Object::Component::Transform>(glm::vec3{4.33f, 1.93f, 1.13f}, glm::vec3{1.0f},
                                                      glm::quat{-0.57f, -0.14f, 0.78f, -0.19f});
    camera.AddComponent<Object::Component::Camera>();

    auto directionalLight = core.CreateEntity();
    directionalLight.AddComponent<Object::Component::Transform>(glm::vec3(-5, 3, 1.5))
        .SetRotation(0.19305186, 0.7857022, -0.30020833, 0.5052504);
    directionalLight.AddComponent<Object::Component::DirectionalLight>(
        {.color = glm::vec4(0.3f), .projection = glm::orthoLH_ZO(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 50.0f)});

    auto ambientLight = core.CreateEntity();
    ambientLight.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 1.0f, 0.0f));
    ambientLight.AddComponent<Object::Component::AmbientLight>(Object::Component::AmbientLight{glm::vec3(0.1f)});

    auto redPointLight = core.CreateEntity();
    redPointLight.AddComponent<Object::Component::Transform>(glm::vec3(-2.0f, 0.4f, -1.0f));
    redPointLight.AddComponent<Object::Component::PointLight>(
        Object::Component::PointLight{.color = glm::vec3(1.0f, 0.2f, 0.2f),
                                      .intensity = 1000.0f,
                                      .radius = 2.0f,
                                      .falloff = 1.0f});

    auto bluePointLight = core.CreateEntity();
    bluePointLight.AddComponent<Object::Component::Transform>(glm::vec3(2.0f, 0.4f, -1.0f));
    bluePointLight.AddComponent<Object::Component::PointLight>(
        Object::Component::PointLight{.color = glm::vec3(0.2f, 0.2f, 1.0f),
                                      .intensity = 50.0f,
                                      .radius = 2.0f,
                                      .falloff = 1.0f});

    auto greenPointLight = core.CreateEntity();
    greenPointLight.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.4f, 2.0f));
    greenPointLight.AddComponent<Object::Component::PointLight>(
        Object::Component::PointLight{.color = glm::vec3(0.2f, 1.0f, 0.2f),
                                      .intensity = 1.0f,
                                      .radius = 2.0f,
                                      .falloff = 1.0f});
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

    try
    {
        core.Run();
    }
    catch (const GraphicExampleError &e)
    {
        Log::Error(fmt::format("GraphicExampleError: {}", e.what()));
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("Unhandled exception: {}", e.what()));
        return EXIT_FAILURE;
    }

    return 0;
}
