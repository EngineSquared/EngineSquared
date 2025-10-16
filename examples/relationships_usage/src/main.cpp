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

    parent.AddComponent<Relationship::Component::Relationship>(core);
    parent.AddComponent<TestComponent>(core);
    child.AddComponent<Relationship::Component::Relationship>(core);
    child.AddComponent<TestComponent>(core).value = 0;

    Relationship::Utils::SetChildOf(core, child, parent);

    core.RegisterSystem([](Engine::Core &core) {
        core.GetRegistry().view<TestComponent, Relationship::Component::Relationship>().each(
            [&core](TestComponent &testComponent, Relationship::Component::Relationship &relationship) {
                if (relationship.first != Engine::Entity::entity_null_id)
                {
                    auto child = relationship.first;
                    auto &component = child.GetComponents<TestComponent>(core);
                    component.value = testComponent.value * 2;
                }
            });
    });

    core.RunSystems();

    auto &component = child.GetComponents<TestComponent>(core);

    std::cout << "Child's component value: " << component.value << std::endl;

    return 0;
}
