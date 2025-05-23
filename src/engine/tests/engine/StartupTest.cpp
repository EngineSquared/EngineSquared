#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "Startup.hpp"

using namespace ES::Engine;

TEST(Core, StartupScheduler)
{
    Core core;

    int update_count = 0;
    core.RegisterSystem<Scheduler::Startup>([&update_count](const Core &) { update_count++; });

    core.RunSystems();
    core.RunSystems();
    core.RunSystems();
    ASSERT_EQ(update_count, 1);
}
