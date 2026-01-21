#include "utils/Utils.hpp"
#include "Logger.hpp"
#include <fmt/format.h>

auto Relationship::Utils::SetChildOf(Engine::Entity child, Engine::Entity parent) -> void
{
    if (IsChildOf(parent, child))
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

    auto &firstChildRS = parentRS.first->GetComponents<Relationship::Component::Relationship>();
    firstChildRS.prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto Relationship::Utils::IsChildOf(Engine::Entity child, Engine::Entity parent) -> bool
{
    const Relationship::Component::Relationship *childRS =
        child.TryGetComponent<Relationship::Component::Relationship>();
    return childRS && childRS->parent == parent;
}

auto Relationship::Utils::RemoveParent(Engine::Entity child) -> void
{
    std::optional<Engine::Entity> parentOpt = GetParent(child);
    if (!parentOpt.has_value())
    {
        Log::Warn(fmt::format("Entity {} has no parent to remove", child));
        return;
    }
    Engine::Entity parent = parentOpt.value();
    auto &childRS = child.GetComponents<Relationship::Component::Relationship>();
    auto &parentRS = parent.GetComponents<Relationship::Component::Relationship>();

    parentRS.children--;
    if (parentRS.first == child)
    {
        if (!childRS.next.has_value())
        {
            parentRS.first = std::nullopt;
        }
        else
        {
            auto &secondChildRS = childRS.next->GetComponents<Relationship::Component::Relationship>();
            secondChildRS.prev = std::nullopt;
            parentRS.first = childRS.next;
        }
    }

    childRS.parent = std::nullopt;
    if (childRS.prev.has_value())
    {
        childRS.prev->GetComponents<Relationship::Component::Relationship>().next = childRS.next;
    }
    if (childRS.next.has_value())
    {
        childRS.next->GetComponents<Relationship::Component::Relationship>().prev = childRS.prev;
    }
}

auto Relationship::Utils::GetParent(Engine::Entity child) -> std::optional<Engine::Entity>
{
    const Relationship::Component::Relationship &childRS = child.GetComponents<Relationship::Component::Relationship>();
    if (!childRS.parent.has_value())
    {
        return std::nullopt;
    }
    return childRS.parent;
}
