#include "DynamicLibrary.hpp"
#include "entity/Entity.hpp"
#include <gtest/gtest.h>

struct Position {
    float x;
    float y;
};

TEST(Meta, smoke)
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
}
