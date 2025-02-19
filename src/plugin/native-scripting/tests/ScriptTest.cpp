#include <gtest/gtest.h>

#include "Entity.hpp"
#include "NativeScripting.hpp"
#include "Registry.hpp"

class speedManager : public ES::Plugin::NativeScripting::Utils::ScriptableEntity {
  public:
    void OnCreate([[maybe_unused]] const ES::Engine::Registry &registry) { std::cout << "OnCreate called" << std::endl; }

    void OnUpdate(ES::Engine::Registry &registry)
    {
        auto view = registry.GetRegistry().view<float>();

        for (auto entity : view)
        {
            auto &speed = view.get<float>(entity);
            speed = 3.0f;
        }
    }

    void OnDestroy()
    {
        // Left empty as OnDestroy does not perform anything here
    }
};

static void InitPlayer(ES::Engine::Registry &registry)
{
    float speed = 1.0f;

    ES::Engine::Entity playerEntity(registry.CreateEntity());
    auto &scriptComponent =
        playerEntity.AddComponent<ES::Plugin::NativeScripting::Component::NativeScripting>(registry);

    scriptComponent.Bind<speedManager>(registry);
    scriptComponent.Instantiate();
    scriptComponent.OnCreate(scriptComponent.seInstance.get());

    registry.GetRegistry().emplace<float>(playerEntity, speed);
}

static void RunNativeScripts(ES::Engine::Registry &registry)
{
    auto view = registry.GetRegistry().view<ES::Plugin::NativeScripting::Component::NativeScripting>();

    for (auto entity : view)
    {
        auto &scriptComponent = view.get<ES::Plugin::NativeScripting::Component::NativeScripting>(entity);
        if (scriptComponent.seInstance)
        {
            scriptComponent.OnUpdate(scriptComponent.seInstance.get());
        }
    }
}

TEST(NativeScripting, speedManagerScript)
{
    ES::Engine::Registry registry;

    registry.RegisterSystem<ES::Engine::Scheduler::Startup>(InitPlayer);
    registry.RegisterSystem<ES::Engine::Scheduler::Update>(RunNativeScripts);

    testing::internal::CaptureStdout();

    auto view = registry.GetRegistry().view<float>();
    for (auto entity : view)
    {
        auto speed = view.get<float>(entity);
        EXPECT_EQ(speed, 1.0f) << "Initial speed should be 1.0f";
    }

    registry.RunSystems();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "OnCreate called\n");

    registry.RunSystems();

    for (auto entity : view)
    {
        auto speed = view.get<float>(entity);
        EXPECT_EQ(speed, 3.0f) << "Speed should be 3.0f after OnUpdate()";
    }
}
