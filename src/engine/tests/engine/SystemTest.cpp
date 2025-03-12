#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

int a = 0;
int b = 0;
int c = 0;

class TestSystemClass {
  public:
    void operator()(Core &registry) const { a++; }
};

void TestSystemFunction(const Core &) { b++; }

TEST(Systems, Casual)
{
    Core reg;

    // Test for class Systems
    reg.RegisterSystem(TestSystemClass(), TestSystemClass());
    reg.RegisterSystem(TestSystemClass());

    // Test for function Systems
    reg.RegisterSystem(TestSystemFunction, TestSystemFunction);
    reg.RegisterSystem(TestSystemFunction);

    // Test for lambda Systems
    reg.RegisterSystem([](Core &) { c++; }, [](Core &) { c++; });
    reg.RegisterSystem([](Core &) { c++; });

    reg.RunSystems();

    ASSERT_EQ(a, 1);
    ASSERT_EQ(b, 1);
    ASSERT_EQ(c, 3);
}
