#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

// Act as a tag component
struct TempComponent {};

struct TempComponentWithAttribut {
    int a;
};

TEST(Core, TemporaryComponent)
{
    Core core;

    auto entity = core.CreateEntity();

    entity.AddTemporaryComponent<TempComponent>(core);

    ASSERT_TRUE(entity.HasComponents<TempComponent>(core));

    Entity::RemoveTemporaryComponents(core);

    ASSERT_FALSE(entity.HasComponents<TempComponent>(core));

    
    entity.AddTemporaryComponent<TempComponentWithAttribut>(core, 1);

    ASSERT_TRUE(entity.HasComponents<TempComponentWithAttribut>(core));
    ASSERT_EQ(entity.GetComponents<TempComponentWithAttribut>(core).a, 1);

    Entity::RemoveTemporaryComponents(core);

    ASSERT_FALSE(entity.HasComponents<TempComponentWithAttribut>(core));
}
