#include <gtest/gtest.h>

#include "component/Relationship.hpp"
#include "core/Core.hpp"
#include "utils/Utils.hpp"

struct TestComponent {
    int value = 10;
};

TEST(Relationship, initialization)
{
    Engine::Core core;

    auto entity = core.CreateEntity();

    auto &relationship = entity.AddComponent<Relationship::Component::Relationship>();

    ASSERT_TRUE(relationship.children == 0);
    ASSERT_FALSE(relationship.first.has_value());
    ASSERT_FALSE(relationship.prev.has_value());
    ASSERT_FALSE(relationship.next.has_value());
    ASSERT_FALSE(relationship.parent.has_value());
}

TEST(Relationship, one_child)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    ASSERT_FALSE(Relationship::Utils::IsChildOf(child, parent));

    Relationship::Utils::SetChildOf(child, parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(child, parent));
    Relationship::Utils::RemoveParent(child);

    ASSERT_FALSE(Relationship::Utils::IsChildOf(child, parent));
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
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first.has_value(), false);
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::SetChildOf(child1, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 1);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child1);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::SetChildOf(child2, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 2);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child2);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::SetChildOf(child3, parent);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 3);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child3);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::RemoveParent(child2);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 2);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child3);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::RemoveParent(child3);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 1);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first, child1);
    ASSERT_TRUE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child3, parent));

    Relationship::Utils::RemoveParent(child1);

    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().children, 0);
    ASSERT_EQ(parent.GetComponents<Relationship::Component::Relationship>().first.has_value(), false);
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child1, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child2, parent));
    ASSERT_FALSE(Relationship::Utils::IsChildOf(child3, parent));
}

TEST(Relationship, remove_parent)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    ASSERT_FALSE(Relationship::Utils::IsChildOf(child, parent));

    Relationship::Utils::RemoveParent(child);

    ASSERT_FALSE(Relationship::Utils::IsChildOf(child, parent));

    Relationship::Utils::SetChildOf(child, parent);
    Relationship::Utils::SetChildOf(child, parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(child, parent));

    Relationship::Utils::RemoveParent(parent);

    ASSERT_TRUE(Relationship::Utils::IsChildOf(child, parent));

    ASSERT_EQ(Relationship::Utils::GetParent(child), parent);
}

TEST(Relationship, get_parent_of_child_without_relationship_component)
{
    Engine::Core core;

    auto child = core.CreateEntity();

    ASSERT_EQ(Relationship::Utils::GetParent(child), std::nullopt);
}

TEST(Relationship, for_each_child)
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

    Relationship::Utils::SetChildOf(child1, parent);
    Relationship::Utils::SetChildOf(child2, parent);
    Relationship::Utils::SetChildOf(child3, parent);

    std::vector<Engine::Entity> children;
    Relationship::Utils::ForEachChild(parent, [&children](Engine::Entity child) { children.push_back(child); });

    ASSERT_EQ(children.size(), 3);
    ASSERT_EQ(children[0], child3);
    ASSERT_EQ(children[1], child2);
    ASSERT_EQ(children[2], child1);
}

TEST(Relationship, try_get_child_components)
{
    Engine::Core core;

    auto child1 = core.CreateEntity();
    auto child2 = core.CreateEntity();
    auto child3 = core.CreateEntity();
    auto parent = core.CreateEntity();

    child1.AddComponent<Relationship::Component::Relationship>();
    child1.AddComponent<TestComponent>(TestComponent{.value = 42});
    child2.AddComponent<Relationship::Component::Relationship>();
    child3.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    Relationship::Utils::SetChildOf(child1, parent);
    Relationship::Utils::SetChildOf(child2, parent);
    Relationship::Utils::SetChildOf(child3, parent);

    auto childComponents = Relationship::Utils::GetChildComponents<TestComponent>(parent);

    ASSERT_EQ(childComponents.size(), 1);
    ASSERT_EQ(childComponents[0].get().value, 42);
}

TEST(Relationship, try_get_parent_component)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<TestComponent>(TestComponent{.value = 42});

    Relationship::Utils::SetChildOf(child, parent);

    auto *parentComponent = Relationship::Utils::TryGetParentComponent<TestComponent>(child);

    ASSERT_NE(parentComponent, nullptr);
    ASSERT_EQ(parentComponent->value, 42);
}

TEST(Relationship, try_get_parent_component_no_parent)
{
    Engine::Core core;

    auto child = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();

    auto *parentComponent = Relationship::Utils::TryGetParentComponent<TestComponent>(child);

    ASSERT_EQ(parentComponent, nullptr);
}

TEST(Relationship, try_get_parent_component_parent_has_no_component)
{
    Engine::Core core;

    auto child = core.CreateEntity();
    auto parent = core.CreateEntity();

    child.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    Relationship::Utils::SetChildOf(child, parent);

    auto *parentComponent = Relationship::Utils::TryGetParentComponent<TestComponent>(child);

    ASSERT_EQ(parentComponent, nullptr);
}

TEST(Relationship, try_get_child_components_no_children)
{
    Engine::Core core;

    auto parent = core.CreateEntity();

    parent.AddComponent<Relationship::Component::Relationship>();

    auto childComponents = Relationship::Utils::GetChildComponents<TestComponent>(parent);

    ASSERT_TRUE(childComponents.empty());
}

TEST(Relationship, try_get_child_components_children_have_no_component)
{
    Engine::Core core;

    auto child1 = core.CreateEntity();
    auto child2 = core.CreateEntity();
    auto parent = core.CreateEntity();

    child1.AddComponent<Relationship::Component::Relationship>();
    child2.AddComponent<Relationship::Component::Relationship>();
    parent.AddComponent<Relationship::Component::Relationship>();

    Relationship::Utils::SetChildOf(child1, parent);
    Relationship::Utils::SetChildOf(child2, parent);

    auto childComponents = Relationship::Utils::GetChildComponents<TestComponent>(parent);

    ASSERT_TRUE(childComponents.empty());
}