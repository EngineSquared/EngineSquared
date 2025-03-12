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
    Core reg;

    reg.RegisterResource<A>({});
    reg.RegisterResource<B>({});
    reg.RegisterResource<C>({});

    // Test for class Systems
    reg.RegisterSystem(TestSystemClass(), TestSystemClass());
    reg.RegisterSystem(TestSystemClass());

    // Test for function Systems
    reg.RegisterSystem(TestSystemFunction, TestSystemFunction);
    reg.RegisterSystem(TestSystemFunction);

    // Test for lambda Systems
    reg.RegisterSystem([](Core &core) { core.GetResource<C>().value++; }, [](Core &core) { core.GetResource<C>().value++; });
    reg.RegisterSystem([](Core &core) { core.GetResource<C>().value++; }); 

    reg.RunSystems();

    ASSERT_EQ(reg.GetResource<A>().value, 1);
    ASSERT_EQ(reg.GetResource<B>().value, 1);
    ASSERT_EQ(reg.GetResource<C>().value, 3);
}
