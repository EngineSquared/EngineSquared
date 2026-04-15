#include "Engine.hpp"
namespace Nc {
#include <ncurses.h>
} // namespace Nc

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

class NcursesPlugin : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~NcursesPlugin() = default;

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

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &) { Nc::initscr(); });

        RegisterSystems<Engine::Scheduler::Shutdown>([](Engine::Core &) { Nc::endwin(); });
    }
};

struct Player {
    int x;
    int y;
};

class App : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~App() = default;

    void Bind(void) override
    {
        RegisterResource(Player{Nc::LINES / 2, Nc::COLS / 2});

        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &player = core.GetResource<Player>();

            int input = Nc::wgetch(Nc::stdscr);
            switch (input)
            {
            case 'w': player.y--; break;
            case 's': player.y++; break;
            case 'a': player.x--; break;
            case 'd': player.x++; break;
            case 'q': core.Stop();
            }
        });

        RegisterSystems<Render>([](Engine::Core &core) {
            auto &player = core.GetResource<Player>();
            Nc::move(player.y, player.x);
        });
    }
};
} // namespace Example

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Example::NcursesPlugin>();

    core.AddPlugins<Example::App>();

    core.Run();

    return 0;
}
