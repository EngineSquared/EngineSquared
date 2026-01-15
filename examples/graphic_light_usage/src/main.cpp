/**************************************************************************
 * EngineSquared - Graphic Usage Example
 *
 * This example demonstrates how to use the Graphic plugin.
 **************************************************************************/

#include "Engine.hpp"

#include "CameraMovement.hpp"
#include "DefaultPipeline.hpp"
#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

void Setup(Engine::Core &core)
{
    auto cube = core.CreateEntity();
    cube.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, 0.0f));
    cube.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());

    auto plane = core.CreateEntity();
    plane.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, -1.0f, 0.0f));
    plane.AddComponent<Object::Component::Mesh>(core, Object::Utils::GeneratePlaneMesh(5.0f, 5.0f, 10u, 10u));

    auto camera = core.CreateEntity();
    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, -5.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
    cameraManager.SetMovementSpeed(3.0f);

    auto ambientLight = core.CreateEntity();
    ambientLight.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 1.0f, 0.0f));
    ambientLight.AddComponent<Object::Component::AmbientLight>(core, Object::Component::AmbientLight{glm::vec3(0.1f)});

    auto redPointLight = core.CreateEntity();
    redPointLight.AddComponent<Object::Component::Transform>(core, glm::vec3(-2.0f, 0.4f, -1.0f));
    redPointLight.AddComponent<Object::Component::PointLight>(
        core, Object::Component::PointLight{.color = glm::vec3(1.0f, 0.2f, 0.2f),
                                            .intensity = 1000.0f,
                                            .radius = 2.0f,
                                            .falloff = 1.0f});

    auto bluePointLight = core.CreateEntity();
    bluePointLight.AddComponent<Object::Component::Transform>(core, glm::vec3(2.0f, 0.4f, -1.0f));
    bluePointLight.AddComponent<Object::Component::PointLight>(
        core, Object::Component::PointLight{.color = glm::vec3(0.2f, 0.2f, 1.0f),
                                            .intensity = 50.0f,
                                            .radius = 2.0f,
                                            .falloff = 1.0f});

    auto greenPointLight = core.CreateEntity();
    greenPointLight.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.4f, 2.0f));
    greenPointLight.AddComponent<Object::Component::PointLight>(
        core, Object::Component::PointLight{.color = glm::vec3(0.2f, 1.0f, 0.2f),
                                            .intensity = 1.0f,
                                            .radius = 2.0f,
                                            .falloff = 1.0f});

    core.RegisterSystem(EscapeKeySystem);

    core.GetResource<Input::Resource::InputManager>().RegisterKeyCallback(
        [](Engine::Core &core, int key, int, int action, int) {
            if (!(action == GLFW_PRESS))
                return;

            auto lightView = core.GetRegistry().view<Object::Component::AmbientLight>();
            if (lightView.empty())
                return;

            Engine::Entity ambientLightEntity = lightView.front();
            auto &ambientLightColor = ambientLightEntity.GetComponents<Object::Component::AmbientLight>(core).color;
            if (key == GLFW_KEY_R)
            {
                ambientLightColor += glm::vec3(0.1f);
            }
            else if (key == GLFW_KEY_F)
            {
                ambientLightColor -= glm::vec3(0.1f);
            }
        });
    core.GetResource<Input::Resource::InputManager>().RegisterKeyCallback(
        [](Engine::Core &core, int key, int, int action, int) {
            if (!(action == GLFW_PRESS))
                return;

            core.GetRegistry().view<Object::Component::PointLight, Object::Component::Transform>().each(
                [key](Object::Component::PointLight &light, Object::Component::Transform &transform) {
                    if (key == GLFW_KEY_T)
                    {
                        light.intensity += 0.5f;
                    }
                    else if (key == GLFW_KEY_G)
                    {
                        light.intensity = std::max(0.0f, light.intensity - 0.5f);
                    }
                });
        });

    core.GetResource<Input::Resource::InputManager>().RegisterKeyCallback(
        [](Engine::Core &core, int key, int, int action, int) {
            if (!(action == GLFW_PRESS))
                return;

            core.GetRegistry().view<Object::Component::PointLight, Object::Component::Transform>().each(
                [key](Object::Component::PointLight &light, Object::Component::Transform &transform) {
                    if (key == GLFW_KEY_Y)
                    {
                        transform.SetPosition(transform.GetPosition() + glm::vec3(0.0f, 0.5f, 0.0f));
                    }
                    else if (key == GLFW_KEY_H)
                    {
                        transform.SetPosition(transform.GetPosition() - glm::vec3(0.0f, 0.5f, 0.0f));
                    }
                });
        });
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, Graphic::Plugin, Input::Plugin, CameraMovement::Plugin>();

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
        core.RunCore();
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
