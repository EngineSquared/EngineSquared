/**************************************************************************
 * EngineSquared - Graphic Plugin Usage With Physics SoftBody Example
 *
 * This example demonstrates how to use the Graphic and Physics plugins together.
 **************************************************************************/

#include "Engine.hpp"

#include "CameraMovement.hpp"
#include "DefaultPipeline.hpp"
#include "Graphic.hpp"
#include "Input.hpp"
#include "Object.hpp"
#include "Physics.hpp"
#include "RenderingPipeline.hpp"
#include "plugin/PluginWindow.hpp"
#include "resource/Window.hpp"

#include "export.h"

constexpr std::string_view FILES_PATH =
#ifdef PATH_ASSETS
    PATH_ASSETS;
#else
    "./assets/";
#endif

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
    // Create floor like Jolt samples: position at Y=-1, half-extent Y=1, so surface is at Y=0
    auto floor =
        Object::Helper::CreatePlane(core, {.width = 200.0f, .depth = 200.0f, .position = glm::vec3(0.0f, -1.0f, 0.0f)});

    // Half-extent 100x1x100 like Jolt samples (surface at Y = -1 + 1 = 0)
    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(100.0f, 1.0f, 100.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(boxCollider);

    floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());
}

void CreateFallingCube(Engine::Core &core, float x, float y, float z, float mass)
{
    auto cube = Object::Helper::CreateCube(core, {.size = 1.0f, .position = glm::vec3(x, y, z)});

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(mass);
    rigidBody.friction = 0.5f;
    rigidBody.restitution = 0.3f;
    cube.AddComponent<Physics::Component::RigidBody>(rigidBody);

    Object::Component::Material texture;
    texture.diffuseTexName = std::string(FILES_PATH) + "texture.png";
    cube.AddComponent<Object::Component::Material>(std::move(texture));
}

void CreateSoftbodyFromOBJ(Engine::Core &core)
{
    Object::OBJLoader loader(std::string(FILES_PATH) + "teapot.obj");
    auto mesh = loader.GetMesh();

    // Create entity with Transform that includes scale
    // The original teapot coords are quite large, so we scale it down
    const float scaleFactor = 0.05f;

    auto teapot = core.CreateEntity();
    // Position, scale, and rotation are now handled by Transform
    // SoftBodySystem will automatically apply the scale to vertices
    teapot.AddComponent<Object::Component::Transform>(
        Object::Component::Transform(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(scaleFactor)));

    // Add Mesh (no manual scaling needed - Transform.scale will be applied by SoftBodySystem)
    teapot.AddComponent<Object::Component::Mesh>(std::move(mesh));

    // Configure soft body settings - SoftBody auto-initializes invMasses and edges from Mesh
    auto settings = Physics::Component::SoftBodySettings::Balloon(5000.0f);
    settings.edgeCompliance = 1.0e-5f;  // Very stiff edges
    settings.shearCompliance = 1.0e-5f; // Very stiff shear
    settings.bendCompliance = 1.0e-4f;  // Stiff bending
    settings.solverIterations = 10;     // More iterations for stability
    settings.vertexRadius = 0.1f;       // Collision margin
    settings.gravityFactor = 1.0f;
    settings.friction = 0.5f;
    settings.restitution = 0.3f;

    // New simplified API: just pass settings, auto-detect Mesh
    teapot.AddComponent<Physics::Component::SoftBody>(Physics::Component::SoftBody(settings));
}

void CreateJellyCube(Engine::Core &core, const glm::vec3 &position, float size, uint32_t gridSize)
{
    // Use Object::Helper to create volumetric jelly cube mesh
    auto jellyCube = Object::Helper::CreateJellyCube(core, {.size = size, .gridSize = gridSize, .position = position});

    // Configure jelly settings
    auto settings = Physics::Component::SoftBodySettings::Jelly();
    settings.edgeCompliance = 1.0e-4f;  // Stiff edges
    settings.shearCompliance = 1.0e-4f; // Stiff shear
    settings.bendCompliance = 1.0e-3f;  // Some bending
    settings.solverIterations = 10;     // More iterations for stability
    settings.vertexRadius = 0.05f;      // Collision margin
    settings.gravityFactor = 1.0f;
    settings.friction = 0.5f;
    settings.restitution = 0.3f;

    // Add SoftBody - auto-initializes from Mesh
    jellyCube.AddComponent<Physics::Component::SoftBody>(Physics::Component::SoftBody(settings));

    Object::Component::Material mat;
    mat.diffuseTexName = std::string(FILES_PATH) + "texture.png";
    jellyCube.AddComponent<Object::Component::Material>(mat);
}

void CreateClothDemo(Engine::Core &core, const glm::vec3 &position)
{
    const uint32_t width = 50u;
    const uint32_t height = 50u;

    // Rotation to map cloth (generated in XY plane) into a vertical YZ plane (rotate +90° around Y)
    const auto rotation = glm::quat(glm::vec3(0.0f, glm::radians(90.0f), 0.0f));

    auto cloth = Object::Helper::CreateCloth(
        core, {.width = width, .height = height, .spacing = 0.20f, .position = position, .rotation = rotation});

    // Configure cloth settings
    auto settings = Physics::Component::SoftBodySettings::Cloth(0.5f);
    settings.solverIterations = 8;
    settings.vertexRadius = 0.02f;

    auto &soft = cloth.AddComponent<Physics::Component::SoftBody>(Physics::Component::SoftBody(settings));

    for (uint32_t x = 0u; x < width; ++x)
    {
        soft.PinVertex(x);
    }

    Object::Component::Material mat;
    mat.diffuseTexName = std::string(FILES_PATH) + "texture.png";
    cloth.AddComponent<Object::Component::Material>(mat);
}

void Setup(Engine::Core &core)
{
    CreateFloor(core);
    CreateFallingCube(core, 5.0f, 10.0f, 0.0f, 2.0f);
    CreateSoftbodyFromOBJ(core);
    CreateJellyCube(core, glm::vec3(-5.0f, 10.0f, 0.0f), 1.0f, 5);
    CreateClothDemo(core, glm::vec3(5.0f, 12.0f, 0.0f));

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 5.0f, -15.0f));
    camera.AddComponent<Object::Component::Camera>();

    auto pointLight = core.CreateEntity();
    pointLight.AddComponent<Object::Component::Transform>(glm::vec3(5.0f, 10.0f, -5.0f));
    pointLight.AddComponent<Object::Component::PointLight>(glm::vec3(0.7f, 0.7f, 0.7f), 1.f, 50.f, 0.1f);

    auto ambientLight = core.CreateEntity();
    ambientLight.AddComponent<Object::Component::AmbientLight>(glm::vec3(0.2f));

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
    cameraManager.SetMovementSpeed(10.0f);

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
            [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *, // NOSONAR
               WGPU_NULLABLE void *) {                                                               // NOSONAR
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
