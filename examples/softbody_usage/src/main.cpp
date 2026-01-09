/**************************************************************************
 * EngineSquared - Graphic Plugin Usage With Physics Example
 *
 * This example demonstrates how to use the Graphic and Physics plugins together.
 **************************************************************************/

#include "Engine.hpp"

#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Physics.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"

#include "export.h"

#ifdef PATH_ASSETS
#    define FILES_PATH PATH_ASSETS
#else
#    define FILES_PATH "./assets/"
#endif

void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

void CameraTranslationSystem(Engine::Core &core)
{
    const float cameraTranslationSpeed = 1.f;
    const float deltaTime = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    auto cameraView = core.GetRegistry().view<Object::Component::Camera>();
    if (cameraView.empty())
        return;
    Engine::Entity camera = cameraView.front();

    glm::vec3 direction{0.0f};
    if (inputManager.IsKeyPressed(GLFW_KEY_W))
    {
        direction += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_S))
    {
        direction += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_A))
    {
        direction += glm::vec3(-1.0f, 0.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_D))
    {
        direction += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_SPACE))
    {
        direction += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
    {
        direction += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    if (glm::length(direction) > 0.0f)
    {
        direction = glm::normalize(direction);
    }
    camera.GetComponents<Object::Component::Transform>(core).SetPosition(
        camera.GetComponents<Object::Component::Transform>(core).GetPosition() +
        direction * cameraTranslationSpeed * deltaTime);
}

void CameraRotationSystem(Engine::Core &core)
{
    const float cameraRotationSpeed = 45.f;
    const float deltaTime = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    auto cameraView = core.GetRegistry().view<Object::Component::Camera>();
    if (cameraView.empty())
        return;
    Engine::Entity camera = cameraView.front();

    glm::vec2 rotation{0.0f};
    if (inputManager.IsKeyPressed(GLFW_KEY_UP))
    {
        rotation.x += 1.0f;
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_DOWN))
    {
        rotation.x -= 1.0f;
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_LEFT))
    {
        rotation.y += 1.0f;
    }
    if (inputManager.IsKeyPressed(GLFW_KEY_RIGHT))
    {
        rotation.y -= 1.0f;
    }
    camera.GetComponents<Object::Component::Transform>(core).SetRotation(
        camera.GetComponents<Object::Component::Transform>(core).GetRotation() *
        glm::quat(glm::vec3(glm::radians(cameraRotationSpeed * deltaTime * rotation.x),
                            glm::radians(cameraRotationSpeed * deltaTime * rotation.y), 0.0f)));
}

void CreateFloor(Engine::Core &core)
{
    // Create floor like Jolt samples: position at Y=-1, half-extent Y=1, so surface is at Y=0
    auto floor = Object::Helper::CreatePlane(core, 200.0f, 200.0f, glm::vec3(0.0f, -1.0f, 0.0f));

    // Half-extent 100x1x100 like Jolt samples (surface at Y = -1 + 1 = 0)
    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(100.0f, 1.0f, 100.0f));
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

    Object::Component::Material texture;
    texture.ambientTexName = FILES_PATH "texture.png";
    cube.AddComponent<Object::Component::Material>(core, std::move(texture));
}

void CreateSoftbodyFromOBJ(Engine::Core &core)
{
    Object::OBJLoader loader(FILES_PATH "teapot.obj");
    auto mesh = loader.GetMesh();

    // Scale down the teapot (original coords are ~40 units)
    const float scaleFactor = 0.05f; // Results in ~2 unit teapot
    for (auto &vertex : mesh.vertices)
        vertex *= scaleFactor;

    // Normalize normals after scaling
    for (auto &normal : mesh.normals)
        normal = glm::normalize(normal);

    auto teapot = core.CreateEntity();
    teapot.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 10.0f, 0.0f));

    // Configure soft body settings
    auto settings = Physics::Component::SoftBodySettings::Balloon(5000.0f);
    settings.edgeCompliance = 1.0e-5f;  // Very stiff edges
    settings.shearCompliance = 1.0e-5f; // Very stiff shear
    settings.bendCompliance = 1.0e-4f;  // Stiff bending
    settings.solverIterations = 10;     // More iterations for stability
    settings.vertexRadius = 0.1f;       // Collision margin
    settings.gravityFactor = 1.0f;
    settings.friction = 0.5f;
    settings.restitution = 0.3f;

    auto soft = Physics::Component::SoftBody::CreateFromMesh(mesh, settings);

    teapot.AddComponent<Object::Component::Mesh>(core, std::move(mesh));
    teapot.AddComponent<Physics::Component::SoftBody>(core, std::move(soft));
    teapot.AddComponent<Object::Component::Material>(core);
}

void Setup(Engine::Core &core)
{
    CreateFloor(core);
    // CreateFallingCube(core, 5.0f, 10.0f, 0.0f, 2.0f);
    CreateSoftbodyFromOBJ(core);

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 5.0f, -10.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    core.RegisterSystem(EscapeKeySystem);
    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(CameraTranslationSystem);
    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(CameraRotationSystem);
}

class GraphicExampleError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, Graphic::Plugin, Input::Plugin, Physics::Plugin>();

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
