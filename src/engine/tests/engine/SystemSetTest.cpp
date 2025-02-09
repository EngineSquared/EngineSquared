#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Core.hpp"

#include "Update.hpp"

using namespace ES::Engine;

static int count = 0;
static bool first_did_run = false;
static bool second_did_run = false;

// This test aims to check if, when adding multiple systems as a systemset,
// they are called in the order they were added and they are not interrupted by other systems.

// This system should always be called first.
void TestSystemSetFirst(const Core &)
{
    count++;
    bool is_valid = count == 1 || count == 3;
    ASSERT_TRUE(is_valid);
    first_did_run = true;
}

// This system should always be called second.
void TestSystemSetSecond(const Core &)
{
    count++;
    bool is_valid = count == 2 || count == 4;
    ASSERT_TRUE(is_valid);
    second_did_run = true;
}

// This system should NOT run between one of the two systems in the systemset.
// It could be run before or after the systemset, so we check if either both or none of the flags are set.
void TestOtherSystem(const Core &)
{
    bool is_valid = first_did_run == second_did_run;
    ASSERT_TRUE(is_valid);
}

TEST(Core, SystemSet)
{
    Core reg;

    reg.RegisterSystem(TestSystemSetFirst, TestSystemSetSecond);

    reg.RegisterSystem(TestOtherSystem);

    reg.RunSystems();

    ASSERT_TRUE(second_did_run);

    first_did_run = false;
    second_did_run = false;

    reg.RunSystems();

    ASSERT_TRUE(second_did_run);
}
