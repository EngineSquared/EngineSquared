#include "Engine.hpp"

// RESOURCES
struct DirectorResource {
    DirectorResource() = delete;
    explicit DirectorResource(std::string_view name_) : name(name_)
    {
        Log::Info(fmt::format("[{:5}]: Arrived at the school.", name));
    }

    int hours = 0;
    std::string name;
};

// COMPONENTS
struct NameComponent {
    std::string value;
};

struct WorkTimeComponent {
    const int maxHours = 8;
    int hours = 0;
};

// SYSTEMS
class DirectorMonitorSystem {
  public:
    void operator()(Engine::Core &core) const
    {
        const auto &director = core.GetResource<DirectorResource>();
        Log::Info(fmt::format("[{:5}]: Monitoring the school activities.", director.name));
    }
};

void HelloSystem(Engine::Core &core)
{
    core.GetRegistry().view<NameComponent>().each(
        [](NameComponent &name) { Log::Info(fmt::format("[{:5}]: Arrived at the school.", name.value)); });
}

class WorkHoursUpdateSystem {
  public:
    void operator()(Engine::Core &core) const
    {
        core.GetRegistry().view<WorkTimeComponent, NameComponent>().each(
            [&core](Engine::EntityId entityId, WorkTimeComponent &workTime, NameComponent &name) {
                Engine::Entity entity{core, entityId};
                if (workTime.hours >= workTime.maxHours)
                {
                    Log::Info(fmt::format("[{:5}]: Work time is over! Going home!", name.value));
                    entity.RemoveComponent<WorkTimeComponent>();
                }
                else
                {
                    workTime.hours += 1;
                }
            });
    }
};

void CloseSystem(Engine::Core &core)
{
    auto view = core.GetRegistry().view<WorkTimeComponent>();
    if (view.front().IsNull())
    {
        core.Stop();
    }
}
void WorkingSystem(Engine::Core &core)
{
    core.GetRegistry().view<NameComponent, WorkTimeComponent>().each(
        [](NameComponent &name, auto &) { Log::Info(fmt::format("[{:5}]: Working...", name.value)); });
}

void GoodbyeSystem(Engine::Core &core)
{
    Log::Info("Everyone left the school. Shutting down the lights and locking the doors.");
}

int main(void)
{
    Engine::Core core;

    core.RegisterResource(DirectorResource{"The A"});

    core.RegisterSystem<Engine::Scheduler::Startup>(HelloSystem);
    core.RegisterSystem<Engine::Scheduler::Update>(WorkingSystem);
    core.RegisterSystem(WorkHoursUpdateSystem(), CloseSystem);
    core.RegisterSystem<Engine::Scheduler::Shutdown>(GoodbyeSystem);

    auto [directorSystemId] = core.RegisterSystem<Engine::Scheduler::Update>(DirectorMonitorSystem());

    core.RegisterSystem<Engine::Scheduler::Update>([&directorSystemId](Engine::Core &core) {
        auto &director = core.GetResource<DirectorResource>();
        director.hours += 1;
        if (director.hours == 2)
        {
            Log::Info(fmt::format("[{:5}]: Going to a meeting.", director.name));
            core.GetScheduler<Engine::Scheduler::Update>().Disable(directorSystemId);
        }
        else if (director.hours == 5)
        {
            Log::Info(fmt::format("[{:5}]: Back from the meeting.", director.name));
            core.GetScheduler<Engine::Scheduler::Update>().Enable(directorSystemId);
        }
        else if (director.hours == 7)
        {
            Log::Info(fmt::format("[{:5}]: Leaving the school.", director.name));
            core.GetScheduler<Engine::Scheduler::Update>().Remove(directorSystemId);
        }
    });

    auto tom = core.CreateEntity();
    tom.AddComponent<NameComponent>("Tom");
    tom.AddComponent<WorkTimeComponent>();

    auto pugo = core.CreateEntity();
    pugo.AddComponent<NameComponent>("Pugo");
    pugo.AddComponent<WorkTimeComponent>({.maxHours = 6});

    core.RunCore();

    return 0;
}
