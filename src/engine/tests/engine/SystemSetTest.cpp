#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Registry.hpp"

#include "Update.hpp"

using namespace ES::Engine;

static int count = 0;
static bool did_run = false;

void TestSystemSetFirst(const Registry &)
{
    count++;
    bool is_valid = count == 1 || count == 3;
    ASSERT_TRUE(is_valid);
}

void TestSystemSetSecond(const Registry &)
{
    count++;
    bool is_valid = count == 2 || count == 4;
    ASSERT_TRUE(is_valid);
    did_run = true;
}

class RunTwiceUpdate : public Scheduler::Update {
  public:
    using USystem = std::function<void(Registry &)>;

    explicit RunTwiceUpdate(Registry &reg) : Scheduler::Update(reg) {}

    void RunSystems(std::vector<USystem> systems) final
    {
        Scheduler::Update::RunSystems(systems);
        Scheduler::Update::RunSystems(systems);
    }
};

TEST(Registry, SystemSet)
{
    Registry reg;

    reg.RegisterScheduler<RunTwiceUpdate>();

    reg.RegisterSystem<RunTwiceUpdate>(TestSystemSetFirst, TestSystemSetSecond);

    reg.RunSystems();

    ASSERT_TRUE(did_run);
}
