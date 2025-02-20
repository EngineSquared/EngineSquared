#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

// Act as a tag component
struct TempComponent {};

TEST(Core, TemporaryComponent)
{
    Core reg;

    auto entity = Entity(reg.CreateEntity());

    entity.AddTemporaryComponent<TempComponent>(reg);

    ASSERT_TRUE(entity.HasComponents<TempComponent>(reg));

    Entity::RemoveTemporaryComponents(reg);

    ASSERT_FALSE(entity.HasComponents<TempComponent>(reg));
}
