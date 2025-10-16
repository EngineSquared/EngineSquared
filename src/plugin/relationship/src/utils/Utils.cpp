#include "utils/Utils.hpp"
#include "Logger.hpp"
#include <fmt/format.h>

// Temp
#include <iostream>

auto Relationship::Utils::SetChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> void
{
    if (IsChildOf(core, parent, child))
    {
        Log::Warn(fmt::format("Entity {} is already a child of the parent {}", Engine::Entity::entity_id_type(child),
                              Engine::Entity::entity_id_type(parent)));
        return;
    }
    auto &parentRS = parent.AddComponentIfNotExists<Relationship::Component::Relationship>(core);
    auto &newChildRS = child.AddComponentIfNotExists<Relationship::Component::Relationship>(core);

    parentRS.children++;
    if (parentRS.children == 1)
    {
        parentRS.first = child;
        newChildRS.parent = parent;
        return;
    }

    auto &firstChildRS = parentRS.first.GetComponents<Relationship::Component::Relationship>(core);
    firstChildRS.prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto Relationship::Utils::IsChildOf(Engine::Core &core, Engine::Entity child, Engine::Entity parent) -> bool
{
    const Relationship::Component::Relationship *childRS =
        child.TryGetComponent<Relationship::Component::Relationship>(core);
    return childRS && childRS->parent == parent;
}

auto Relationship::Utils::RemoveParent(Engine::Core &core, Engine::Entity child) -> void
{
    Engine::Entity parent = GetParent(core, child);
    if (parent == Engine::Entity::entity_null_id)
        return;
    auto &childRS = child.GetComponents<Relationship::Component::Relationship>(core);
    auto &parentRS = parent.GetComponents<Relationship::Component::Relationship>(core);

    parentRS.children--;
    if (parentRS.first == child)
    {
        if (childRS.next == Engine::Entity::entity_null_id)
        {
            parentRS.first = Engine::Entity::entity_null_id;
        }
        else
        {
            auto &secondChildRS = childRS.next.GetComponents<Relationship::Component::Relationship>(core);
            secondChildRS.prev = Engine::Entity::entity_null_id;
            parentRS.first = childRS.next;
        }
    }

    childRS.parent = Engine::Entity::entity_null_id;
    if (childRS.prev != Engine::Entity::entity_null_id)
    {
        childRS.prev.GetComponents<Relationship::Component::Relationship>(core).next = childRS.next;
    }
    if (childRS.next != Engine::Entity::entity_null_id)
    {
        childRS.next.GetComponents<Relationship::Component::Relationship>(core).prev = childRS.prev;
    }
}

auto Relationship::Utils::GetParent(Engine::Core &core, Engine::Entity child) -> Engine::Entity
{
    const Relationship::Component::Relationship &childRS =
        child.GetComponents<Relationship::Component::Relationship>(core);

    if (childRS.parent == Engine::Entity::entity_null_id)
    {
        Log::Warn(fmt::format("Entity {} has no parent", Engine::Entity::entity_id_type(child)));
        return Engine::Entity::entity_null_id;
    }
    return childRS.parent;
}
