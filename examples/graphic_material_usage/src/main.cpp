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
    // Option to lock the cursor to the window
    // auto &window = core.GetResource<Window::Resource::Window>(); //NOSONAR
    // window.MaskCursor(); //NOSONAR

    // Default Material
    auto cube = core.CreateEntity();
    cube.AddComponent<Object::Component::Transform>(core, glm::vec3(-2.0f, 0.0f, 0.0f));
    cube.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());

    // Custom Material with Texture
    Object::Component::Material materialWithTexture;
    materialWithTexture.ambientTexName = "./asset/texture.png";
    auto cube1 = core.CreateEntity();
    cube1.AddComponent<Object::Component::Transform>(core);
    cube1.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());
    cube1.AddComponent<Object::Component::Material>(core, std::move(materialWithTexture));

    // Custom Material without Texture
    Object::Component::Material materialWithoutTexture;
    auto cube2 = core.CreateEntity();
    cube2.AddComponent<Object::Component::Transform>(core, glm::vec3(2.0f, 0.0f, 0.0f));
    cube2.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());
    cube2.AddComponent<Object::Component::Material>(core, std::move(materialWithoutTexture));

    // Camera
    auto camera = core.CreateEntity();
    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, -5.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
    cameraManager.SetMovementSpeed(3.0f);

    core.RegisterSystem(EscapeKeySystem);
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
