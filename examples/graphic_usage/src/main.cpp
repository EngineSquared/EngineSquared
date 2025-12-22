/**************************************************************************
 * EngineSquared - Graphic Usage Example
 *
 * This example demonstrates how to use the Graphic plugin.
 **************************************************************************/

#include "Engine.hpp"

#include "Graphic.hpp"
#include "plugin/PluginWindow.hpp"
#include "Object.hpp"
#include "RenderingPipeline.hpp"
#include "Input.hpp"

void Setup(Engine::Core &core)
{
    auto cube = core.CreateEntity();

    cube.AddComponent<Object::Component::Transform>(core);
    cube.AddComponent<Object::Component::Mesh>(core, Object::Utils::GenerateCubeMesh());

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, 2.0f));
    camera.AddComponent<Object::Component::Camera>(core);

    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
        [](Engine::Core &core) {
            const float cameraTranslationSpeed = 1.f;
            const float cameraRotationSpeed = 45.f;
            const float deltaTime = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
            auto &inputManager = core.GetResource<Input::Resource::InputManager>();

            Engine::Entity camera = core.GetRegistry().view<Object::Component::Camera>().front();

            if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE)) {
                core.Stop();
            }
            glm::vec3 direction{0.0f};
            if (inputManager.IsKeyPressed(GLFW_KEY_W)) {
                direction += glm::vec3(0.0f, 0.0f, -1.0f);
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_S)) {
                direction += glm::vec3(0.0f, 0.0f, 1.0f);
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_A)) {
                direction += glm::vec3(1.0f, 0.0f, 0.0f);
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_D)) {
                direction += glm::vec3(-1.0f, 0.0f, 0.0f);
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_SPACE)) {
                direction += glm::vec3(0.0f, 1.0f, 0.0f);
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                direction += glm::vec3(0.0f, -1.0f, 0.0f);
            }
            if (glm::length(direction) > 0.0f) {
                direction = glm::normalize(direction);
            }
            camera.GetComponents<Object::Component::Transform>(core).position +=
                cameraTranslationSpeed * deltaTime * direction;

            glm::vec2 rotation{0.0f};
            if (inputManager.IsKeyPressed(GLFW_KEY_UP)) {
                rotation.x += 1.0f;
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_DOWN)) {
                rotation.x -= 1.0f;
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_LEFT)) {
                rotation.y += 1.0f;
            }
            if (inputManager.IsKeyPressed(GLFW_KEY_RIGHT)) {
                rotation.y -= 1.0f;
            }
            camera.GetComponents<Object::Component::Transform>(core).rotation *=
                glm::quat(glm::vec3(glm::radians(cameraRotationSpeed * deltaTime * rotation.x), glm::radians(cameraRotationSpeed * deltaTime * rotation.y), 0.0f));
        }
    );
}

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, Graphic::Plugin, Input::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>(
        [](Engine::Core &core){
                core.GetResource<Graphic::Resource::GraphicSettings>().SetOnErrorCallback(
                [](WGPUDevice const *, WGPUErrorType type, WGPUStringView message, WGPU_NULLABLE void *, WGPU_NULLABLE void *) {
                    Log::Error(fmt::format("Custom uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                                        std::string(message.data, message.length)));
                    throw std::runtime_error("Custom uncaptured device error occurred");
                });
        }
    );

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);

    core.RunCore();

    return 0;
}
