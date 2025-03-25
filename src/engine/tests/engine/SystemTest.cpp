#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

struct A {
    int value = 0;
};

struct B {
    int value = 0;
};

struct C {
    int value = 0;
};

class TestSystemClass {
  public:
    void operator()(Core &core) const { core.GetResource<A>().value++; }
};

void TestSystemFunction(Core &core) { core.GetResource<B>().value++; }

TEST(Systems, Casual)
{
    Core core;

    core.RegisterResource<A>({});
    core.RegisterResource<B>({});
    core.RegisterResource<C>({});

    // Test for class Systems
    core.RegisterSystem(TestSystemClass(), TestSystemClass());
    core.RegisterSystem(TestSystemClass());

    // Test for function Systems
    core.RegisterSystem(TestSystemFunction, TestSystemFunction);
    core.RegisterSystem(TestSystemFunction);

    // Test for lambda Systems
    core.RegisterSystem([](Core &core) { core.GetResource<C>().value++; },
                        [](Core &core) { core.GetResource<C>().value++; });
    core.RegisterSystem([](Core &core) { core.GetResource<C>().value++; });

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 1);
    ASSERT_EQ(core.GetResource<B>().value, 1);
    ASSERT_EQ(core.GetResource<C>().value, 3);
}
