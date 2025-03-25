#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

TEST(Core, CreateEntity)
{
    Core core;

    core.CreateEntity();
}

TEST(Core, Systems)
{
    Core core;

    int x = 42;

    core.RegisterSystem([&x](const Core &) { x = 69; });

    core.RegisterSystem([&x](const Core &) { x = 144; });

    ASSERT_EQ(x, 42);

    core.RunSystems();

    ASSERT_EQ(x, 144);
}

TEST(Core, Resources)
{
    Core core;

    struct Res {
        int x;
    };

    core.RegisterResource<Res>({42});

    core.RegisterSystem([](Core &core) { core.GetResource<Res>().x = 69; });

    ASSERT_EQ(core.GetResource<Res>().x, 42);

    core.RunSystems();

    ASSERT_EQ(core.GetResource<Res>().x, 69);
}
