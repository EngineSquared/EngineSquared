#include "DynamicLibrary.hpp"
#include "entity/Entity.hpp"
#include <gtest/gtest.h>

// struct ZigPosition {
//     float x;
//     float y;
// };

// struct RustPosition {
//     float x;
//     float y;
// };

struct CppPosition {
    float x;
    float y;
};

struct CPosition {
    float x;
    float y;
};

TEST(DynamicLibrary, Smoke)
{
    Engine::Core core;

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.AddPlugins<DynamicLibrary::Plugin>();

    // core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("ZigPosition",
    //                                                                           entt::type_hash<ZigPosition>::value());
    // core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("RustPosition",
    //                                                                           entt::type_hash<RustPosition>::value());
    core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("CppPosition",
                                                                              entt::type_hash<CppPosition>::value());
    core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("CPosition",
                                                                              entt::type_hash<CPosition>::value());

    auto entity1 = core.CreateEntity();
    auto entity2 = core.CreateEntity();

    // entity1.AddComponent<ZigPosition>(69.0f, 42.0f);
    // entity2.AddComponent<ZigPosition>(67.0f, 123456789.0f);

    // entity1.AddComponent<RustPosition>(69.0f, 42.0f);
    // entity2.AddComponent<RustPosition>(67.0f, 123456789.0f);

    entity1.AddComponent<CppPosition>(69.0f, 42.0f);
    entity2.AddComponent<CppPosition>(67.0f, 123456789.0f);

    entity1.AddComponent<CPosition>(69.0f, 42.0f);
    entity2.AddComponent<CPosition>(67.0f, 123456789.0f);

    core.RunSystems();

    // ASSERT_EQ(entity1.GetComponents<ZigPosition>().x, 0.0f);
    // ASSERT_EQ(entity1.GetComponents<ZigPosition>().y, 1.0f);
    // ASSERT_EQ(entity2.GetComponents<ZigPosition>().x, 2.0f);
    // ASSERT_EQ(entity2.GetComponents<ZigPosition>().y, 3.0f);

    // ASSERT_EQ(entity1.GetComponents<RustPosition>().x, 0.0f);
    // ASSERT_EQ(entity1.GetComponents<RustPosition>().y, 1.0f);
    // ASSERT_EQ(entity2.GetComponents<RustPosition>().x, 2.0f);
    // ASSERT_EQ(entity2.GetComponents<RustPosition>().y, 3.0f);

    ASSERT_EQ(entity1.GetComponents<CppPosition>().x, 0.0f);
    ASSERT_EQ(entity1.GetComponents<CppPosition>().y, 1.0f);
    ASSERT_EQ(entity2.GetComponents<CppPosition>().x, 2.0f);
    ASSERT_EQ(entity2.GetComponents<CppPosition>().y, 3.0f);

    ASSERT_EQ(entity1.GetComponents<CPosition>().x, 0.0f);
    ASSERT_EQ(entity1.GetComponents<CPosition>().y, 1.0f);
    ASSERT_EQ(entity2.GetComponents<CPosition>().x, 2.0f);
    ASSERT_EQ(entity2.GetComponents<CPosition>().y, 3.0f);
}
