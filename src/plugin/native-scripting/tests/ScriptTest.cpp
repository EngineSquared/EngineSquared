#include <gtest/gtest.h>

#include "Entity.hpp"
#include "NativeScripting.hpp"
#include "Core.hpp"
#include "ScriptingSystem.hpp"

class speedManager : public ES::Plugin::NativeScripting::Utils::ScriptableEntity {
  public:
    void OnCreate([[maybe_unused]] const ES::Engine::Core &core)
    {
        std::cout << "OnCreate called" << std::endl;
    }

    void OnUpdate(ES::Engine::Core &core)
    {
        auto view = core.GetRegistry().view<float>();

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

static void InitPlayer(ES::Engine::Core &core)
{
    float speed = 1.0f;

    ES::Engine::Entity playerEntity(core.CreateEntity());
    auto &scriptComponent =
        playerEntity.AddComponent<ES::Plugin::NativeScripting::Component::NativeScripting>(core);

    scriptComponent.Bind<speedManager>(core);

    core.GetRegistry().emplace<float>(playerEntity, speed);
}

TEST(NativeScripting, speedManagerScript)
{
    ES::Engine::Core core;

    core.RegisterSystem<ES::Engine::Scheduler::Startup>(InitPlayer);
    core.RegisterSystem<ES::Engine::Scheduler::Update>(ES::Plugin::NativeScripting::System::UpdateScripts);

    testing::internal::CaptureStdout();

    core.RunSystems();

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "OnCreate called\n");

    auto view = core.GetRegistry().view<float>();
    for (auto entity : view)
    {
        auto speed = view.get<float>(entity);
        EXPECT_EQ(speed, 3.0f) << "Speed should be 3.0f after OnUpdate()";
    }
}
