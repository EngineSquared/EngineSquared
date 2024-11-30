#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Registry.hpp"
#include "Startup.hpp"

using namespace ES::Engine;

TEST(Registry, StartupScheduler)
{
    Registry reg;

    int update_count = 0;
    reg.RegisterSystem<Scheduler::Startup>([&](Registry &registry) { update_count++; });

    reg.RunSystems();
    reg.RunSystems();
    reg.RunSystems();
    ASSERT_EQ(update_count, 1);
}
