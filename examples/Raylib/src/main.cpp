#include "Engine.hpp"
namespace Rl {
#include <raylib.h>
}

namespace Example {
class PreRender : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class Render : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class PostRender : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class RaylibPlugin : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~RaylibPlugin() = default;

    void Bind(void) override
    {
        RegisterScheduler<PreRender>();
        RegisterScheduler<Render>();
        RegisterScheduler<PostRender>();

        this->GetCore().SetSchedulerAfter<PreRender, Engine::Scheduler::Update>();
        this->GetCore().SetSchedulerAfter<Render, PreRender>();
        this->GetCore().SetSchedulerAfter<PostRender, Render>();
        this->GetCore().SetSchedulerBefore<PostRender, Engine::Scheduler::Shutdown>();
        // Startup -> (Update -> PreRender -> Render -> PostRender) -> Shutdown

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &) {
            const int screenWidth = 800;
            const int screenHeight = 450;
            Rl::InitWindow(screenWidth, screenHeight, "Engine² x Raylib");
            Rl::SetTargetFPS(60);
        });

        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            if (Rl::WindowShouldClose())
            {
                core.Stop();
            }
        });

        RegisterSystems<PreRender>([](Engine::Core &) { Rl::BeginDrawing(); });

        RegisterSystems<PostRender>([](Engine::Core &) { Rl::EndDrawing(); });

        RegisterSystems<Engine::Scheduler::Shutdown>([](Engine::Core &) { Rl::CloseWindow(); });
    }
};

class App : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~App() = default;

    void Bind(void) override
    {
        RegisterSystems<Render>([](Engine::Core &) {
            Rl::ClearBackground(Rl::RAYWHITE);
            Rl::DrawText("Congrats! You created your first window!", 190, 200, 20, Rl::LIGHTGRAY);
        });
    }
};
} // namespace Example

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Example::RaylibPlugin>();

    core.AddPlugins<Example::App>();

    core.Run();

    return 0;
}
