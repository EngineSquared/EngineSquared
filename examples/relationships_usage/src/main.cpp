#include "Engine.hpp"
#include "Relationship.hpp"

struct TestComponent {
    int value = 10;
};

struct NameComponent {
    std::string name;
};

int main(void)
{
    Engine::Core core;

    auto parent = core.CreateEntity();
    auto child1 = core.CreateEntity();
    auto child1_1 = core.CreateEntity();
    auto child2 = core.CreateEntity();
    auto child2_1 = core.CreateEntity();
    auto child3 = core.CreateEntity();
    auto child4 = core.CreateEntity();

    parent.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<NameComponent>({.name = "Tom"});
    parent.AddComponent<TestComponent>();
    child1.AddComponent<Relationship::Component::Relationship>();
    child1.AddComponent<NameComponent>({.name = "Alexandre"});
    child1_1.AddComponent<Relationship::Component::Relationship>();
    child1_1.AddComponent<NameComponent>({.name = "Yuna"});
    child2.AddComponent<Relationship::Component::Relationship>();
    child2.AddComponent<NameComponent>({.name = "Dorvann"});
    child2_1.AddComponent<Relationship::Component::Relationship>();
    child2_1.AddComponent<NameComponent>({.name = ".exe"});
    child3.AddComponent<Relationship::Component::Relationship>();
    child3.AddComponent<NameComponent>({.name = "Lucas"});
    child4.AddComponent<Relationship::Component::Relationship>();
    child4.AddComponent<NameComponent>({.name = "Guillaume"});
    child4.AddComponent<TestComponent>().value = 0;

    Relationship::Utils::SetChildOf(child1, parent);
    Relationship::Utils::SetChildOf(child1_1, child1);
    Relationship::Utils::SetChildOf(child2, parent);
    Relationship::Utils::SetChildOf(child2_1, child2);
    Relationship::Utils::SetChildOf(child3, parent);
    Relationship::Utils::SetChildOf(child4, parent);

    core.RegisterSystem([](Engine::Core &core) {
        core.GetRegistry().view<TestComponent, Relationship::Component::Relationship>().each(
            [&core](TestComponent &testComponent, Relationship::Component::Relationship &relationship) {
                if (relationship.first.has_value())
                {
                    auto &child = relationship.first.value();
                    auto &component = child.GetComponents<TestComponent>();
                    component.value = testComponent.value;
                }
            });
    });

    core.RegisterSystem([](Engine::Core &core) {
        core.GetRegistry().view<Relationship::Component::Relationship, NameComponent>().each(
            [&core](Engine::EntityId entityId, Relationship::Component::Relationship &relationship,
                    NameComponent &name) {
                Log::Info(fmt::format("{} has {} children/s:", name.name, relationship.children));
                Relationship::Utils::ForEachChild(Engine::Entity{core, entityId}, [&core](Engine::Entity child) {
                    auto &childName = child.GetComponents<NameComponent>();
                    Log::Info(fmt::format(" - {}", childName.name));
                });
            });
    });

    core.RunSystems();

    auto &component = child4.GetComponents<TestComponent>();

    Log::Info(fmt::format("Child's component value: {}", component.value));

    return 0;
}
