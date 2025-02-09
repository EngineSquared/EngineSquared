#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Core.hpp"

using namespace ES::Engine;

TEST(Core, CreateEntity)
{
    Core reg;

    reg.CreateEntity();
}

TEST(Core, Systems)
{
    Core reg;

    int x = 42;

    reg.RegisterSystem([&x](const Core &) { x = 69; });

    reg.RegisterSystem([&x](const Core &) { x = 144; });

    ASSERT_EQ(x, 42);

    reg.RunSystems();

    ASSERT_EQ(x, 144);
}

TEST(Core, Resources)
{
    Core reg;

    struct Res {
        int x;
    };

    reg.RegisterResource<Res>({42});

    reg.RegisterSystem([](Core &registry) { registry.GetResource<Res>().x = 69; });

    ASSERT_EQ(reg.GetResource<Res>().x, 42);

    reg.RunSystems();

    ASSERT_EQ(reg.GetResource<Res>().x, 69);
}
