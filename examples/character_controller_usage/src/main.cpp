/**************************************************************************
 * EngineSquared - CharacterController Usage Example
 *
 * This example demonstrates how to set up a player-like entity using the
 * Physics CharacterController component with a capsule collider.
 **************************************************************************/

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

#include <iostream>

struct CharacterControllerExampleRuntime {
    uint32_t step = 0;
    Engine::EntityId player = Engine::EntityId::Null;
};

void SetupCharacterControllerScene(Engine::Core &core)
{
    auto floor =
        Object::Helper::CreatePlane(core, {.width = 20.0f, .depth = 20.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
    floor.AddComponent<Physics::Component::BoxCollider>(glm::vec3(10.0f, 0.5f, 10.0f));
    floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

    auto player = core.CreateEntity();
    player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 4.0f, 0.0f));
    player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
    player.AddComponent<Physics::Component::CharacterController>(Physics::Component::CharacterController::Create(70.0f));

    core.RegisterResource(CharacterControllerExampleRuntime{.step = 0, .player = player.GetId()});

    std::cout << "CharacterController scene created." << std::endl;
}

void DriveCharacterController(Engine::Core &core)
{
    auto &runtime = core.GetResource<CharacterControllerExampleRuntime>();
    auto player = Engine::Entity(core, runtime.player);
    if (player.IsNull())
    {
        core.Stop();
        return;
    }

    auto &controller = player.GetComponents<Physics::Component::CharacterController>();
    runtime.step++;

    if (runtime.step < 120)
    {
        controller.linearVelocity = glm::vec3(0.0f, controller.linearVelocity.y, 1.5f);
    }
    else if (runtime.step == 120)
    {
        controller.linearVelocity = glm::vec3(0.0f, 5.0f, 0.0f);
    }
    else
    {
        controller.linearVelocity = glm::vec3(0.0f, controller.linearVelocity.y, 0.0f);
    }

    const auto &transform = player.GetComponents<Object::Component::Transform>();
    const auto position = transform.GetPosition();

    if (runtime.step % 30 == 0)
    {
        std::cout << "step=" << runtime.step << " position=(" << position.x << ", " << position.y << ", " << position.z
                  << ") velocity=(" << controller.linearVelocity.x << ", " << controller.linearVelocity.y << ", "
                  << controller.linearVelocity.z << ")" << std::endl;
    }

    if (runtime.step >= 240)
        core.Stop();
}

int main(void)
{
    std::cout << "========================================" << std::endl;
    std::cout << "  EngineSquared - Character Controller Example" << std::endl;
    std::cout << "========================================" << std::endl;

    Engine::Core core;
    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(SetupCharacterControllerScene);
    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(DriveCharacterController);
    core.RunCore();

    return 0;
}
