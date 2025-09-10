#include <gtest/gtest.h>

#include "component/Relationship.hpp"
#include "core/Core.hpp"
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

TEST(Relationship, one_child)
{
    ES::Engine::Core core;

    ES::Engine::Entity child = core.CreateEntity();
    ES::Engine::Entity parent = core.CreateEntity();

    child.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    parent.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);

    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ES::Plugin::Relationship::Utils::SetChildOf(core, child, parent);

    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, child);

    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));
}

TEST(Relationship, multiple_children)
{
    ES::Engine::Core core;

    ES::Engine::Entity child1 = core.CreateEntity();
    ES::Engine::Entity child2 = core.CreateEntity();
    ES::Engine::Entity child3 = core.CreateEntity();
    ES::Engine::Entity parent = core.CreateEntity();

    child1.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    child2.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    child3.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    parent.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 0);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first,
              ES::Engine::Entity::entity_null_id);
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::SetChildOf(core, child1, parent);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 1);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first, child1);
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::SetChildOf(core, child2, parent);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 2);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first, child2);
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::SetChildOf(core, child3, parent);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 3);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first, child3);
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, child2);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 2);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first, child3);
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, child3);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 1);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first, child1);
    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, child1);

    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).children, 0);
    ASSERT_EQ(parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).first,
              ES::Engine::Entity::entity_null_id);
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child3, parent));
}

TEST(Relationship, remove_parent)
{
    ES::Engine::Core core;

    ES::Engine::Entity child = core.CreateEntity();
    ES::Engine::Entity parent = core.CreateEntity();

    child.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    parent.AddComponent<ES::Plugin::Relationship::Component::Relationship>(core);

    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, child);

    ASSERT_FALSE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ES::Plugin::Relationship::Utils::SetChildOf(core, child, parent);
    ES::Plugin::Relationship::Utils::SetChildOf(core, child, parent);

    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ES::Plugin::Relationship::Utils::RemoveParent(core, parent);

    ASSERT_TRUE(ES::Plugin::Relationship::Utils::IsChildOf(core, child, parent));

    ASSERT_EQ(ES::Plugin::Relationship::Utils::GetParent(core, child), parent);
}
