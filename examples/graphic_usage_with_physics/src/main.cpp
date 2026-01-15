/**************************************************************************
 * EngineSquared - Graphic Plugin Usage With Physics Example
 *
 * This example demonstrates how to use the Graphic and Physics plugins together.
 **************************************************************************/

#include "Engine.hpp"

#include "CameraMovement.hpp"
#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "DefaultPipeline.hpp"
#include "Physics.hpp"
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

void CreateFloor(Engine::Core &core)
{
    // Align visual plane with physics surface at y = 0.0
    auto floor = Object::Helper::CreatePlane(core, 20.0f, 20.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 1.0f, 10.0f));
    // Shift collider down so its top surface lies at y = 0.0 (matches the plane visual)
    boxCollider.offset = glm::vec3(0.0f, -1.0f, 0.0f);
    floor.AddComponent<Physics::Component::BoxCollider>(core, boxCollider);

    floor.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());
}

void CreateFallingCube(Engine::Core &core, float x, float y, float z, float mass)
{
    auto cube = Object::Helper::CreateCube(core, 1.0f, glm::vec3(x, y, z));

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(mass);
    rigidBody.friction = 0.5f;
    rigidBody.restitution = 0.3f;
    cube.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
}

void CreateBouncyBall(Engine::Core &core, float x, float y, float z)
{
    auto ball = Object::Helper::CreateSphere(core, 0.5f, glm::vec3(x, y, z));

    auto collider = Physics::Component::SphereCollider(0.5f);
    ball.AddComponent<Physics::Component::SphereCollider>(core, collider);

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rigidBody.restitution = 0.8f;
    rigidBody.friction = 0.2f;
    ball.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
}

void CreateCharacterCapsule(Engine::Core &core, float x, float y, float z)
{
    // Create a visual capsule that matches the physics capsule (cylinder height = 1.5, radius = 0.25)
    auto character = Object::Helper::CreateCapsule(core, 0.25f, 1.5f, glm::vec3(x, y, z), 32u, 4u);

    auto capsuleCollider = Physics::Component::CapsuleCollider(0.75f, 0.25f);
    character.AddComponent<Physics::Component::CapsuleCollider>(core, capsuleCollider);

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(5.0f);
    rigidBody.friction = 0.8f;
    rigidBody.restitution = 0.2f;
    rigidBody.angularDamping = 0.5f;
    character.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
}

void Setup(Engine::Core &core)
{
    CreateFloor(core);
    CreateFallingCube(core, 0.0f, 10.0f, 0.0f, 2.0f);
    CreateBouncyBall(core, 2.0f, 15.0f, 0.0f);
    CreateCharacterCapsule(core, -2.0f, 5.0f, 0.0f);

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 1.0f, -10.0f));
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

    core.AddPlugins<Window::Plugin, DefaultPipeline::Plugin, Input::Plugin, CameraMovement::Plugin, Physics::Plugin>();

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

    core.RunCore();

    return 0;
}
