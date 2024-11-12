#include <gtest/gtest.h>

#include "Entity.hpp"
#include "Registry.hpp"

using namespace ES::Engine;

// Act as a tag component
struct TempComponent {};

TEST(Registry, TemporaryComponent)
{
    Registry reg;

    Entity entity = Entity(reg.CreateEntity());

    entity.AddTemporaryComponent<TempComponent>(reg);

    ASSERT_TRUE(entity.HasComponents<TempComponent>(reg));

    Entity::RemoveTemporaryComponents(reg);

    ASSERT_FALSE(entity.HasComponents<TempComponent>(reg));
}
