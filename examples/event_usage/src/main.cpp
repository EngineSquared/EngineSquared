#include "Engine.hpp"
#include "Event.hpp"

struct EventExample {
    std::string value;
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Event::Plugin>();

    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    eventManager.RegisterCallback<EventExample>([](const EventExample &event) {
        Log::Info(fmt::format("{:12}: received event with value: {}", "Immediate", event.value));
    });

    eventManager.RegisterCallback<EventExample, Engine::Scheduler::Update>([](const EventExample &event) {
        Log::Info(fmt::format("{:12}: received event with value: {}", "Synchronised", event.value));
    });

    auto id = eventManager.RegisterCallback<EventExample>(
        [](const EventExample &) { Log::Error("This callback should never be called"); });

    eventManager.UnregisterCallback<EventExample>(id);

    EventExample summit{"Epitech Summit"};
    EventExample rncp1{"RNCP 1"};
    EventExample rncp2{"RNCP 2"};

    eventManager.PushEvent<EventExample>(summit);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    eventManager.PushEvent<EventExample>(rncp1);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    eventManager.PushEvent<EventExample>(rncp2);

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    core.RunSystems();

    return 0;
}
