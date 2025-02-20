#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "Startup.hpp"

using namespace ES::Engine;

TEST(Core, StartupScheduler)
{
    Core reg;

    int update_count = 0;
    reg.RegisterSystem<Scheduler::Startup>([&update_count](const Core &) { update_count++; });

    reg.RunSystems();
    reg.RunSystems();
    reg.RunSystems();
    ASSERT_EQ(update_count, 1);
}
