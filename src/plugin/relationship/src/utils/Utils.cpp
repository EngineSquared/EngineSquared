#include "utils/Utils.hpp"
#include "Logger.hpp"
#include <fmt/format.h>

// Temp
#include <iostream>

auto Plugin::Relationship::Utils::SetChildOf(Engine::Core &core, Engine::Entity child,
                                                 Engine::Entity parent) -> void
{
    if (IsChildOf(core, parent, child))
    {
        Log::Warn(fmt::format("Entity {} is already a child of the parent {}",
                                         Engine::Entity::entity_id_type(child),
                                         Engine::Entity::entity_id_type(parent)));
        return;
    }
    auto &parentRS = parent.AddComponentIfNotExists<Plugin::Relationship::Component::Relationship>(core);
    auto &newChildRS = child.AddComponentIfNotExists<Plugin::Relationship::Component::Relationship>(core);

    parentRS.children++;
    if (parentRS.children == 1)
    {
        parentRS.first = child;
        newChildRS.parent = parent;
        return;
    }

    auto &firstChildRS = parentRS.first.GetComponents<Plugin::Relationship::Component::Relationship>(core);
    firstChildRS.prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto Plugin::Relationship::Utils::IsChildOf(Engine::Core &core, Engine::Entity child,
                                                Engine::Entity parent) -> bool
{
    const Plugin::Relationship::Component::Relationship *childRS =
        child.TryGetComponent<Plugin::Relationship::Component::Relationship>(core);
    return childRS && childRS->parent == parent;
}

auto Plugin::Relationship::Utils::RemoveParent(Engine::Core &core, Engine::Entity child) -> void
{
    Engine::Entity parent = GetParent(core, child);
    if (parent == Engine::Entity::entity_null_id)
        return;
    auto &childRS = child.GetComponents<Plugin::Relationship::Component::Relationship>(core);
    auto &parentRS = parent.GetComponents<Plugin::Relationship::Component::Relationship>(core);

    parentRS.children--;
    if (parentRS.first == child)
    {
        if (childRS.next == Engine::Entity::entity_null_id)
        {
            parentRS.first = Engine::Entity::entity_null_id;
        }
        else
        {
            auto &secondChildRS = childRS.next.GetComponents<Plugin::Relationship::Component::Relationship>(core);
            secondChildRS.prev = Engine::Entity::entity_null_id;
            parentRS.first = childRS.next;
        }
    }

    childRS.parent = Engine::Entity::entity_null_id;
    if (childRS.prev != Engine::Entity::entity_null_id)
    {
        childRS.prev.GetComponents<Plugin::Relationship::Component::Relationship>(core).next = childRS.next;
    }
    if (childRS.next != Engine::Entity::entity_null_id)
    {
        childRS.next.GetComponents<Plugin::Relationship::Component::Relationship>(core).prev = childRS.prev;
    }
}

auto Plugin::Relationship::Utils::GetParent(Engine::Core &core, Engine::Entity child) -> Engine::Entity
{
    const Plugin::Relationship::Component::Relationship &childRS =
        child.GetComponents<Plugin::Relationship::Component::Relationship>(core);

    if (childRS.parent == Engine::Entity::entity_null_id)
    {
        Log::Warn(fmt::format("Entity {} has no parent", Engine::Entity::entity_id_type(child)));
        return Engine::Entity::entity_null_id;
    }
    return childRS.parent;
}
