#include <gtest/gtest.h>

#include "Core.hpp"
#include "component/Relationship.hpp"
#include "utils/Utils.hpp"

TEST(Relationship, initialization)
{
    ES::Engine::Core core;

    ES::Engine::Entity entity = core.CreateEntity();

    auto &relationship = entity.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);

    ASSERT_TRUE(relationship.children == 0);
    ASSERT_FALSE(relationship.first.IsValid());
    ASSERT_FALSE(relationship.prev.IsValid());
    ASSERT_FALSE(relationship.next.IsValid());
    ASSERT_FALSE(relationship.parent.IsValid());
}

TEST(Relationship, add_child)
{
    ES::Engine::Core core;

    ES::Engine::Entity entity1 = core.CreateEntity();
    ES::Engine::Entity entity2 = core.CreateEntity();

    entity1.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    entity2.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);

    ES::Plugin::Relationship::Utils::SetChildOf(core, entity1, entity2);
}


