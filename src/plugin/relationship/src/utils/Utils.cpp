#include <format>
#include "utils/Utils.hpp"
#include "Logger.hpp"

// Temp
#include <iostream>

auto ES::Plugin::Relationship::Utils::SetChildOf(ES::Engine::Core &core, ES::Engine::Entity child,
                                                 ES::Engine::Entity parent) -> void
{
    if (IsChildOf(core, parent, child))
    {
        ES::Utils::Log::Warn(std::format("Entity {} is already a child of the parent {}",
                                         ES::Engine::Entity::entity_id_type(child),
                                         ES::Engine::Entity::entity_id_type(parent)));
        return;
    }
    auto &parentRS = parent.AddComponentIfNotExists<ES::Plugin::Relationship::Component::Relationship>(core);
    auto &newChildRS = child.AddComponentIfNotExists<ES::Plugin::Relationship::Component::Relationship>(core);

    parentRS.children++;
    if (parentRS.children == 1)
    {
        parentRS.first = child;
        newChildRS.parent = parent;
        return;
    }

    auto &firstChildRS = parentRS.first.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);
    firstChildRS.prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto ES::Plugin::Relationship::Utils::IsChildOf(ES::Engine::Core &core, ES::Engine::Entity child,
                                                ES::Engine::Entity parent) -> bool
{
    const ES::Plugin::Relationship::Component::Relationship *childRS = child.TryGetComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    return childRS && childRS->parent == parent;
}

auto ES::Plugin::Relationship::Utils::RemoveParent(ES::Engine::Core &core, ES::Engine::Entity child) -> void
{
    ES::Engine::Entity parent = GetParent(core, child);
    if (parent == ES::Engine::Entity::entity_null_id)
        return;
    auto &childRS = child.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);
    auto &parentRS = parent.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);

    parentRS.children--;
    if (parentRS.first == child)
    {
        if (childRS.next == ES::Engine::Entity::entity_null_id)
        {
            parentRS.first = ES::Engine::Entity::entity_null_id;
        }
        else
        {
            auto &secondChildRS = childRS.next.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);
            secondChildRS.prev = ES::Engine::Entity::entity_null_id;
            parentRS.first = childRS.next;
        }
    }

    childRS.parent = ES::Engine::Entity::entity_null_id;
    if (childRS.prev != ES::Engine::Entity::entity_null_id)
    {
        childRS.prev.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).next = childRS.next;
    }
    if (childRS.next != ES::Engine::Entity::entity_null_id)
    {
        childRS.next.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core).prev = childRS.prev;
    }
}

auto ES::Plugin::Relationship::Utils::GetParent(ES::Engine::Core &core, ES::Engine::Entity child) -> ES::Engine::Entity
{
    const ES::Plugin::Relationship::Component::Relationship &childRS = child.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);

    if (childRS.parent == ES::Engine::Entity::entity_null_id)
    {
        ES::Utils::Log::Warn(std::format("Entity {} has no parent",
                                         ES::Engine::Entity::entity_id_type(child)));
        return ES::Engine::Entity::entity_null_id;
    }
    return childRS.parent;
}