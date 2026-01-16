#include "utils/Utils.hpp"
#include "Logger.hpp"
#include <fmt/format.h>

// Temp
#include <iostream>

auto Relationship::Utils::SetChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> void
{
    if (IsChildOf(core, parent, child))
    {
        Log::Warn(fmt::format("Entity {} is already a child of the parent {}", child, parent));
        return;
    }
    auto &parentRS = parent.AddComponentIfNotExists<Relationship::Component::Relationship>();
    auto &newChildRS = child.AddComponentIfNotExists<Relationship::Component::Relationship>();

    parentRS.children++;
    if (parentRS.children == 1)
    {
        parentRS.first = child;
        newChildRS.parent = parent;
        return;
    }

    auto &firstChildRS = parentRS.first.GetComponents<Relationship::Component::Relationship>();
    firstChildRS.prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto Relationship::Utils::IsChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> bool
{
    const Relationship::Component::Relationship *childRS =
        child.TryGetComponent<Relationship::Component::Relationship>();
    return childRS && childRS->parent == parent;
}

auto Relationship::Utils::RemoveParent(Engine::Core &core, Engine::Entity child) -> void
{
    Engine::Entity parent = GetParent(core, child);
    if (parent == Engine::Entity::Null())
        return;
    auto &childRS = child.GetComponents<Relationship::Component::Relationship>();
    auto &parentRS = parent.GetComponents<Relationship::Component::Relationship>();

    parentRS.children--;
    if (parentRS.first == child)
    {
        if (childRS.next.IsValid())
        {
            parentRS.first = Engine::Entity::Null();
        }
        else
        {
            auto &secondChildRS = childRS.next.GetComponents<Relationship::Component::Relationship>();
            secondChildRS.prev = Engine::Entity::Null();
            parentRS.first = childRS.next;
        }
    }

    childRS.parent = Engine::Entity::Null();
    if (childRS.prev.IsValid())
    {
        childRS.prev.GetComponents<Relationship::Component::Relationship>().next = childRS.next;
    }
    if (childRS.next.IsValid())
    {
        childRS.next.GetComponents<Relationship::Component::Relationship>().prev = childRS.prev;
    }
}

auto Relationship::Utils::GetParent(Engine::Core &core, Engine::Entity child) -> Engine::Entity
{
    const Relationship::Component::Relationship &childRS = child.GetComponents<Relationship::Component::Relationship>();
    if (childRS.parent.IsValid())
    {
        Log::Warn(fmt::format("Entity {} has no parent", child));
        return Engine::Entity::Null();
    }
    return childRS.parent;
}
