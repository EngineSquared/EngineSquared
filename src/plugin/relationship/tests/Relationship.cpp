#include <gtest/gtest.h>

#include "component/Relationship.hpp"
#include "core/Core.hpp"
#include "utils/Utils.hpp"

TEST(Relationship, initialization)
{
    Engine::Core core;

    auto entity = core.CreateEntity();

    auto &relationship = entity.AddComponent<Relationship::Component::Relationship>();

    ASSERT_TRUE(relationship.children == 0);
    ASSERT_FALSE(relationship.first.IsValid());
    ASSERT_FALSE(relationship.prev.IsValid());
    ASSERT_FALSE(relationship.next.IsValid());
    ASSERT_FALSE(relationship.parent.IsValid());
}

TEST(Relationship, one_child)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child, parent));

    Relationship::Utils::SetChildOf(core, child, parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child, parent));

    Relationship::Utils::RemoveParent(core, child);

    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child, parent));
}

TEST(Relationship, multiple_children)
{
    Engine::Core core;

    auto child1 = core.CreateEntity();
    auto child2 = core.CreateEntity();
    auto child3 = core.CreateEntity();
    auto parent = core.CreateEntity();

    child1.AddComponent<Relationship::Component::Relationship>();
    child2.AddComponent<Relationship::Component::Relationship>();
    child3.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 0);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, Engine::Entity::Null());
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::SetChildOf(core, child1, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 1);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child1);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::SetChildOf(core, child2, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 2);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child2);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::SetChildOf(core, child3, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 3);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child3);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::RemoveParent(core, child2);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 2);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child3);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::RemoveParent(core, child3);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 1);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child1);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child3, parent));

    Relationship::Utils::RemoveParent(core, child1);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 0);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, Engine::Entity::Null());
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child3, parent));
}

TEST(Relationship, remove_parent)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child, parent));

    Relationship::Utils::RemoveParent(core, child);

    ASSERT_FALSE(Relationship::Utils::IsChildOf(core, child, parent));

    Relationship::Utils::SetChildOf(core, child, parent);
    Relationship::Utils::SetChildOf(core, child, parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child, parent));

    Relationship::Utils::RemoveParent(core, parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(core, child, parent));

    ASSERT_EQ(Relationship::Utils::GetParent(core, child), parent);
}
