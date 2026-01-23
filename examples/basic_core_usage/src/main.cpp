#include "Engine.hpp"

struct TestComponent {
    int value;
};

void TestSystem(Engine::Core &core)
{
    core.GetRegistry().view<TestComponent>().each(
        [](TestComponent &) { std::cout << "Hello from entity with component" << std::endl; });
}

int main(void)
{
    Engine::Core core;

    auto entity = core.CreateEntity();

    entity.AddComponent<TestComponent>(42);

    core.RegisterSystem(TestSystem);

    core.RunSystems();

    return 0;
}
