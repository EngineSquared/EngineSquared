#include "Engine.hpp"
#include "Relationship.hpp"

struct TestComponent {
    int value = 10;
};

int main(void)
{
    Engine::Core core;

    auto parent = core.CreateEntity();
    auto child = core.CreateEntity();

    parent.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<TestComponent>();
    child.AddComponent<Relationship::Component::Relationship>();
    child.AddComponent<TestComponent>().value = 0;

    Relationship::Utils::SetChildOf(core, child, parent);

    core.RegisterSystem([](Engine::Core &core) {
        core.GetRegistry().view<TestComponent, Relationship::Component::Relationship>().each(
            [&core](TestComponent &testComponent, Relationship::Component::Relationship &relationship) {
                if (relationship.first.IsValid())
                {
                    auto &child = relationship.first;
                    auto &component = child.GetComponents<TestComponent>();
                    component.value = testComponent.value * 2;
                }
            });
    });

    core.RunSystems();

    auto &component = child.GetComponents<TestComponent>();

    std::cout << "Child's component value: " << component.value << std::endl;

    return 0;
}
