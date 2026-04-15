/// @file main.cpp
/// @brief Tutorial for the EngineSquared core lifecycle.
///     The application runs through three phases:
///     - Startup: entities and resources are created, teachers arrive.
///     - Update: teachers work each tick until their hours are exhausted, then leave.
///       The director monitors activities but may be absent for meetings.
///     - Shutdown: the school closes once all teachers have left.

#include "Engine.hpp"

namespace Example {
/// @brief Holds unique data shared across the entire application.
namespace Resource {
/// @struct DirectorResource
/// @brief Simulates a unique director resource shared by all systems.
struct DirectorResource {
    DirectorResource() = delete;
    explicit DirectorResource(std::string_view name_) : name(name_)
    {
        Log::Info(fmt::format("[{:5}]: Arrived at the school.", name));
    }

    uint8_t hours = 0;
    std::string name;
};
} // namespace Resource
namespace Component {
/// @brief Display name assigned to an entity.
struct Name {
    std::string value;
};

/// @struct WorkTime
/// @brief Tracks how many hours an entity has worked. The component is removed from the entity once maxHours is
///     reached.
struct WorkTime {
    const uint8_t maxHours = 8;
    uint8_t hours = 0;
};
} // namespace Component
namespace System {
/// @brief Logs the director's monitoring activity each tick.
class DirectorMonitor {
  public:
    void operator()(Engine::Core &core) const
    {
        const auto &director = core.GetResource<Resource::DirectorResource>();
        Log::Info(fmt::format("[{:5}]: Monitoring the school activities.", director.name));
    }
};

/// @brief Logs a greeting message for each entity carrying a Name component.
void Hello(Engine::Core &core)
{
    core.GetRegistry().view<Component::Name>().each(
        [](Component::Name &name) { Log::Info(fmt::format("[{:5}]: Arrived at the school.", name.value)); });
}

/// @brief Increments work hours for each entity. Removes the WorkTime component when the maximum is reached.
class WorkHoursUpdate {
  public:
    void operator()(Engine::Core &core) const
    {
        core.GetRegistry().view<Component::WorkTime, Component::Name>().each(
            [&core](Engine::EntityId entityId, Component::WorkTime &workTime, Component::Name &name) {
                Engine::Entity entity{core, entityId};
                if (workTime.hours >= workTime.maxHours)
                {
                    Log::Info(fmt::format("[{:5}]: Work time is over! Going home!", name.value));
                    entity.RemoveComponent<Component::WorkTime>();
                }
                else
                {
                    workTime.hours += 1;
                }
            });
    }
};

/// @brief Stops the core loop when no entity carries a WorkTime component anymore.
void Close(Engine::Core &core)
{
    auto view = core.GetRegistry().view<Component::WorkTime>();
    if (view.front().IsNull())
    {
        core.Stop();
    }
}

/// @brief Logs a working message for each entity still carrying a WorkTime component.
void Working(Engine::Core &core)
{
    core.GetRegistry().view<Component::Name, Component::WorkTime>().each(
        [](Component::Name &name, auto &) { Log::Info(fmt::format("[{:5}]: Working...", name.value)); });
}

/// @brief Logs a shutdown message once all entities have left.
void Goodbye(Engine::Core &core)
{
    Log::Info("Everyone left the school. Shutting down the lights and locking the doors.");
}
} // namespace System

class App : public Engine::APlugin {
  public:
    using Engine::APlugin::APlugin;

    void Bind(void) override
    {
        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto tom = core.CreateEntity();
            tom.AddComponent<Component::Name>("Tom");
            tom.AddComponent<Component::WorkTime>();

            auto pugo = core.CreateEntity();
            pugo.AddComponent<Component::Name>("Pugo");
            pugo.AddComponent<Component::WorkTime>({.maxHours = 6});
        });

        RegisterResource(Resource::DirectorResource{"The A"});

        RegisterSystems<Engine::Scheduler::Startup>(System::Hello);
        RegisterSystems<Engine::Scheduler::Update>(System::Working);
        RegisterSystems<Engine::Scheduler::Update>(System::WorkHoursUpdate(), System::Close);
        RegisterSystems<Engine::Scheduler::Shutdown>(System::Goodbye);

        auto [directorSystemId] = RegisterSystems<Engine::Scheduler::Update>(System::DirectorMonitor());

        // Example of scheduler control: disable, enable, then remove a system.
        RegisterSystems<Engine::Scheduler::Update>([directorSystemId](Engine::Core &core) {
            auto &director = core.GetResource<Resource::DirectorResource>();
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
    }
};

} // namespace Example

int main(void)
{
    // Core stores entities, components, resources, and schedulers for the app.
    Engine::Core core;

    // App::Bind registers the example setup (systems + resource) into the core.
    core.AddPlugins<Example::App>();

    /// Run until `System::Close` calls `core.Stop()`.
    core.Run();

    return 0;
}
