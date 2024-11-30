#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Registry.hpp"

using namespace ES::Engine;

TEST(Registry, CreateEntity)
{
    Registry reg;

    reg.CreateEntity();
}

TEST(Registry, Systems)
{
    Registry reg;

    int x = 42;

    reg.RegisterSystem([&x](const Registry &) { x = 69; });

    reg.RegisterSystem([&x](const Registry &) { x = 144; });

    ASSERT_EQ(x, 42);

    reg.RunSystems();

    ASSERT_EQ(x, 144);
}

TEST(Registry, Resources)
{
    Registry reg;

    struct Res {
        int x;
    };

    reg.RegisterResource<Res>({42});

    reg.RegisterSystem([](Registry &registry) { registry.GetResource<Res>().x = 69; });

    ASSERT_EQ(reg.GetResource<Res>().x, 42);

    reg.RunSystems();

    ASSERT_EQ(reg.GetResource<Res>().x, 69);
}

TEST(Registry, FixedUpdate)
{
    Registry reg;

    int x = 17;

    reg.RegisterSystem(
        [&x](const Registry &) {
            x *= 3;
            std::cout << "1";
        },
        ScheduleLabel::FIXED);
    reg.RegisterSystem(
        [&x](const Registry &) {
            x += 3;
            std::cout << "2";
        },
        ScheduleLabel::FIXED);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(reg.GetFixedDeltaTime() * 2.1s);

    testing::internal::CaptureStdout();

    reg.RunSystems();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "1212");

    ASSERT_EQ(x, (17 * 3 + 3) * 3 + 3);
}
