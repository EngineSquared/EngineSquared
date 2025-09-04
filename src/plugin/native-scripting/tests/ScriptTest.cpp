#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "NativeScripting.hpp"

struct ActionHistory {
    std::vector<std::string> actions;
};

class TestScript : public ES::Plugin::NativeScripting::Utils::ScriptableEntity {
  public:
    void OnCreate(ES::Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnCreate"); }

    void OnUpdate(ES::Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnUpdate"); }

    void OnDestroy(ES::Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnDestroy"); }
};

TEST(NativeScripting, CasualUse)
{
    ES::Engine::Core core;

    core.RegisterResource<ActionHistory>({});
    core.AddPlugins<ES::Plugin::NativeScripting::Plugin>();

    auto e = core.CreateEntity();

    e.AddComponent<ES::Plugin::NativeScripting::Component::NativeScripting>(core).Bind<TestScript>(core);

    core.RunSystems();

    auto &actions = core.GetResource<ActionHistory>().actions;

    ASSERT_EQ(actions.size(), 2);
    ASSERT_EQ(actions[0], "OnCreate");
    ASSERT_EQ(actions[1], "OnUpdate");

    core.KillEntity(e);

    ASSERT_EQ(actions.size(), 3);
    ASSERT_EQ(actions[2], "OnDestroy");
}
