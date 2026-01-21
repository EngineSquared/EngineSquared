#include <gtest/gtest.h>

#include "NativeScripting.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"

struct ActionHistory {
    std::vector<std::string> actions;
};

class TestScript : public NativeScripting::Utils::ScriptableEntity {
  public:
    using NativeScripting::Utils::ScriptableEntity::ScriptableEntity;

    void OnCreate(Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnCreate"); }

    void OnUpdate(Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnUpdate"); }

    void OnDestroy(Engine::Core &core) { core.GetResource<ActionHistory>().actions.emplace_back("OnDestroy"); }
};

TEST(NativeScripting, CasualUse)
{
    Engine::Core core;

    core.RegisterResource<ActionHistory>({});
    core.AddPlugins<NativeScripting::Plugin>();

    auto e = core.CreateEntity();

    e.AddComponent<NativeScripting::Component::NativeScripting>().Bind<TestScript>(core);

    core.RunSystems();

    auto &actions = core.GetResource<ActionHistory>().actions;

    ASSERT_EQ(actions.size(), 2);
    ASSERT_EQ(actions[0], "OnCreate");
    ASSERT_EQ(actions[1], "OnUpdate");

    core.KillEntity(e);

    ASSERT_EQ(actions.size(), 3);
    ASSERT_EQ(actions[2], "OnDestroy");
}
