#include "Engine.hpp"
#include "NativeScripting.hpp"

class TestScript : public NativeScripting::Utils::ScriptableEntity {
  public:
    void OnCreate(Engine::Core &core) { std::cout << "Entity Created" << std::endl; }

    void OnUpdate(Engine::Core &core) { std::cout << "Entity Updated" << std::endl; }

    void OnDestroy(Engine::Core &core) { std::cout << "Entity Destroyed" << std::endl; }
};

int main(void)
{
    Engine::Core core;

    core.AddPlugins<NativeScripting::Plugin>();

    auto e = core.CreateEntity();

    e.AddComponent<NativeScripting::Component::NativeScripting>(core).Bind<TestScript>(core);

    for (int i = 0; i < 5; ++i)
    {
        core.RunSystems();
    }

    core.KillEntity(e);

    return 0;
}
