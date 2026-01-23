#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "entity/Entity.hpp"

using namespace Engine;

// Act as a tag component
struct TempComponent {};

struct TempComponentWithAttribut {
    int a;
};

TEST(Core, TemporaryComponent)
{
    Core core;

    auto entity = core.CreateEntity();

    entity.AddTemporaryComponent<TempComponent>();

    ASSERT_TRUE(entity.HasComponents<TempComponent>());

    Entity::RemoveTemporaryComponents(core);

    ASSERT_FALSE(entity.HasComponents<TempComponent>());

    entity.AddTemporaryComponent<TempComponentWithAttribut>(1);

    ASSERT_TRUE(entity.HasComponents<TempComponentWithAttribut>());
    ASSERT_EQ(entity.GetComponents<TempComponentWithAttribut>().a, 1);

    Entity::RemoveTemporaryComponents(core);

    ASSERT_FALSE(entity.HasComponents<TempComponentWithAttribut>());
}
