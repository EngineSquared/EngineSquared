#include "DynamicLibrary.hpp"
#include "entity/Entity.hpp"
#include <gtest/gtest.h>

struct ZigPosition {
    float x;
    float y;
};

struct RustPosition {
    float x;
    float y;
};

TEST(DynamicLibrary, Smoke)
{
    Engine::Core core;

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.AddPlugins<DynamicLibrary::Plugin>();

    core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("ZigPosition",
                                                                              entt::type_hash<ZigPosition>::value());
    core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("RustPosition",
                                                                              entt::type_hash<RustPosition>::value());

    auto entity1 = core.CreateEntity();
    auto entity2 = core.CreateEntity();

    entity1.AddComponent<ZigPosition>(69.0f, 42.0f);
    entity2.AddComponent<ZigPosition>(67.0f, 123456789.0f);

    entity1.AddComponent<RustPosition>(69.0f, 42.0f);
    entity2.AddComponent<RustPosition>(67.0f, 123456789.0f);

    core.RunSystems();

    ASSERT_EQ(entity1.GetComponents<ZigPosition>().x, 0.0f);
    ASSERT_EQ(entity1.GetComponents<ZigPosition>().y, 1.0f);
    ASSERT_EQ(entity2.GetComponents<ZigPosition>().x, 2.0f);
    ASSERT_EQ(entity2.GetComponents<ZigPosition>().y, 3.0f);

    ASSERT_EQ(entity1.GetComponents<RustPosition>().x, 0.0f);
    ASSERT_EQ(entity1.GetComponents<RustPosition>().y, 1.0f);
    ASSERT_EQ(entity2.GetComponents<RustPosition>().x, 2.0f);
    ASSERT_EQ(entity2.GetComponents<RustPosition>().y, 3.0f);
}
