#include "DynamicLibrary.hpp"
#include "entity/Entity.hpp"
#include <gtest/gtest.h>

struct Position {
    float x;
    float y;
};

TEST(DynamicLibrary, Smoke)
{
    Engine::Core core;

    core.AddPlugins<DynamicLibrary::Plugin>();

    core.GetResource<DynamicLibrary::Resource::ComponentsMeta>().AddComponent("Position",
                                                                              entt::type_hash<Position>::value());

    auto entity1 = core.CreateEntity();
    auto entity2 = core.CreateEntity();

    entity1.AddComponent<Position>(69.0f, 42.0f);
    entity2.AddComponent<Position>(67.0f, 123456789.0f);

    core.RunSystems();

    ASSERT_EQ(entity1.GetComponents<Position>().x, 0.0f);
    ASSERT_EQ(entity1.GetComponents<Position>().y, 1.0f);
    ASSERT_EQ(entity2.GetComponents<Position>().x, 2.0f);
    ASSERT_EQ(entity2.GetComponents<Position>().y, 3.0f);
}
