#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "entity/Entity.hpp"

using namespace Engine;

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

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

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

TEST(Systems, EnableDisable)
{
    Core core;

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

    core.SetDefaultScheduler<Scheduler::Update>();

    core.RegisterResource<A>({});
    core.RegisterResource<B>({});
    core.RegisterResource<C>({});

    auto [a, b, c] =
        core.RegisterSystem(TestSystemClass(), TestSystemFunction, [](Core &co) { co.GetResource<C>().value++; });

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 1);
    ASSERT_EQ(core.GetResource<B>().value, 1);
    ASSERT_EQ(core.GetResource<C>().value, 1);

    core.GetScheduler<Scheduler::Update>().Disable(a);
    core.GetScheduler<Scheduler::Update>().Disable(b);
    core.GetScheduler<Scheduler::Update>().Disable(c);

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 1);
    ASSERT_EQ(core.GetResource<B>().value, 1);
    ASSERT_EQ(core.GetResource<C>().value, 1);

    core.GetScheduler<Scheduler::Update>().Enable(a);
    core.GetScheduler<Scheduler::Update>().Enable(b);
    core.GetScheduler<Scheduler::Update>().Enable(c);

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 2);
    ASSERT_EQ(core.GetResource<B>().value, 2);
    ASSERT_EQ(core.GetResource<C>().value, 2);
}

TEST(Systems, ErrorHandling)
{
    Core core;

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterResource<A>({});
    core.RegisterResource<B>({});

    core.RegisterSystemWithErrorHandler([](Core &c) { c.GetResource<A>().value++; },
                                        [](const Core &) { /* Nothing to do here */ });

    core.RegisterSystemWithErrorHandler([](const Core &) { throw std::runtime_error("Test error"); }, // NOSONAR
                                        [](Core &c) { c.GetResource<B>().value++; });

    EXPECT_THROW(core.RunSystems(), std::runtime_error);

    ASSERT_EQ(core.GetResource<A>().value, 1);
    ASSERT_EQ(core.GetResource<B>().value, 1);
}

TEST(Systems, ErrorHandlingDoesNotAllowDuplicates)
{
    Core core;

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterResource<A>({});
    core.RegisterResource<B>({});

    core.RegisterSystemWithErrorHandler(TestSystemClass(), [](const Core &) { /* Nothing to do here */ });
    core.RegisterSystemWithErrorHandler(TestSystemClass(), TestSystemClass());
    core.RegisterSystemWithErrorHandler(TestSystemClass(), TestSystemFunction);

    core.RegisterSystemWithErrorHandler([](const Core &) { throw std::runtime_error("Test error"); }, // NOSONAR
                                        [](Core &c) { c.GetResource<B>().value++; });

    EXPECT_THROW(core.RunSystems(), std::runtime_error);

    ASSERT_EQ(core.GetResource<A>().value, 1);
    ASSERT_EQ(core.GetResource<B>().value, 1);
}

TEST(Systems, SystemCannotBeAddedTwiceAsWrapped)
{
    Core core;

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterResource<A>({});

    core.RegisterSystem(TestSystemClass());
    core.RegisterSystemWithErrorHandler(TestSystemClass(), [](const Core &) { /* Nothing to do here */ });

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 1);
}

TEST(Systems, SystemCannotBeAddedTwiceAsWrapped2)
{
    Core core;

    core.SetErrorPolicyForAllSchedulers(Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterResource<A>({});

    core.RegisterSystemWithErrorHandler(TestSystemClass(), [](const Core &) { /* Nothing to do here */ });
    core.RegisterSystem(TestSystemClass());

    core.RunSystems();

    ASSERT_EQ(core.GetResource<A>().value, 1);
}
