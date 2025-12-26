/**************************************************************************
 * EngineSquared - Graphic Usage Example
 *
 * This example demonstrates how to use the Graphic plugin.
 **************************************************************************/

#include "Engine.hpp"

#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"

struct TargetController {
    Engine::Entity target;

    bool active = false;
    glm::vec2 startMouse = {0.0f, 0.0f};
    glm::quat originRotation = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);

    float sensitivity = 0.005f;
    float scrollSensitivity = 0.1f;
    glm::vec2 velocity = {0.0, 0.0};
    glm::vec2 previousDelta = {0.0, 0.0};
    float inertia = 0.9f;
};

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

static glm::vec3 GetMovementForce(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    glm::vec3 force{0.0f};
    if (inputManager.IsKeyPressed(GLFW_KEY_W))
    {
        force += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_S))
    {
        force += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_A))
    {
        force += glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_D))
    {
        force += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_SPACE))
    {
        force += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        force += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    if (glm::length(force) > 0.0f)
    {
        force = glm::normalize(force);
    }
    return force;
}

void CameraTranslationSystem(Engine::Core &core)
{
    const float cameraTranslationSpeed = 1.f;
    const float deltaTime = core.GetScheduler<Engine::Scheduler::Update>().GetDeltaTime();
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    glm::vec3 movementForce = GetMovementForce(core);
    if (glm::length(movementForce) <= 0.0f)
        return;

    Engine::Entity camera = core.GetRegistry().view<Object::Component::Camera>().front();
    auto &transform = camera.GetComponents<Object::Component::Transform>(core);

    glm::vec3 forwardDir = camera.GetComponents<Object::Component::Transform>(core).GetForwardVector();
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    glm::vec3 rightDir = glm::normalize(glm::cross(up, forwardDir));
    glm::vec3 downDir = glm::normalize(glm::cross(forwardDir, rightDir));
    glm::vec3 movementDirection = forwardDir * movementForce.z * transform.GetScale().z +
                                  downDir * movementForce.y * transform.GetScale().y +
                                  rightDir * movementForce.x * transform.GetScale().x;
    transform.SetPosition(transform.GetPosition() + movementDirection * cameraTranslationSpeed * deltaTime);
}

void CameraRotationSystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    inputManager.RegisterMouseButtonCallback([](Engine::Core &core, int button, int action, int) {
        auto &targetController = core.GetResource<TargetController>();
        auto &camera = targetController.target;
        auto &window = core.GetResource<Window::Resource::Window>();
        glm::vec2 mousePos = window.GetMousePosition();
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            switch (action)
            {
            case GLFW_PRESS:
                targetController.active = true;
                targetController.startMouse = glm::vec2(mousePos.x, -window.GetSize().y + mousePos.y);
                targetController.originRotation =
                    camera.GetComponents<Object::Component::Transform>(core).GetRotation();
                break;
            case GLFW_RELEASE: targetController.active = false; break;
            }
        }
    });
    inputManager.RegisterCursorPosCallback([&](Engine::Core &, double x, double y) {
        auto &targetController = core.GetResource<TargetController>();
        auto &camera = targetController.target;

        if (targetController.active)
        {
            glm::vec2 currentMouse{(float) x, -(float) y};
            glm::vec2 delta = (currentMouse - targetController.startMouse) * targetController.sensitivity;

            glm::quat yawRotation = glm::angleAxis(delta.x, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat pitchRotation = glm::angleAxis(delta.y, glm::vec3(1.0f, 0.0f, 0.0f));

            auto &cameraTransform = camera.GetComponents<Object::Component::Transform>(core);
            cameraTransform.SetRotation(yawRotation * targetController.originRotation * pitchRotation);

            targetController.velocity = (delta - targetController.previousDelta) * 100.0f;
            targetController.previousDelta = delta;
        }
    });
}

void Setup(Engine::Core &core)
{
    // TODO: find why x is inverted
    // Default Material
    auto cube = core.CreateEntity();
    cube.AddComponent<Object::Component::Transform>(core, glm::vec3(-2.0f, 0.0f, 0.0f));
    cube.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());

    // Custom Material with Texture
    Object::Component::Material materialWithTexture;
    Log::Info(std::filesystem::current_path().string());
    materialWithTexture.ambientTexName = "./examples/graphic_material_usage/asset/texture.png";
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
    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, -2.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    auto &targetController = core.GetResource<TargetController>();
    targetController.target = camera;

    core.RegisterSystem(EscapeKeySystem);
    core.RegisterSystem(CameraTranslationSystem);
    core.RegisterSystem<Engine::Scheduler::Startup>(CameraRotationSystem);
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, Graphic::Plugin, Input::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &core) {
        core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *,
               WGPU_NULLABLE void *) {
                Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                       std::string(message.data, message.length)));
                throw GraphicExampleError("Custom uncaptured device error occurred");
            });
    });

    core.RegisterResource(TargetController{});
    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.RunCore();

    return 0;
}
