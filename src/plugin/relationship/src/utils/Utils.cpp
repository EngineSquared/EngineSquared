#include "utils/Utils.hpp"
#include "Logger.hpp"

auto ES::Plugin::Relationship::Utils::SetChildOf(ES::Engine::Core &core, ES::Engine::Entity parent,
                                                 ES::Engine::Entity child) -> void
{
    auto &parentRS = parent.AddComponentIfNotExists<ES::Plugin::Relationship::Component::Relationship>(core);
    auto &newChildRS = child.AddComponentIfNotExists<ES::Plugin::Relationship::Component::Relationship>(core);

    if (newChildRS.parent == parent)
    {
        ES::Utils::Log::Warn("Entity " + std::to_string(ES::Engine::Entity::entity_id_type(child)) +
                             " is already a child of the parent " +
                             std::to_string(ES::Engine::Entity::entity_id_type(parent)));
        return;
    }

    parentRS.children++;
    if (parentRS.children == 1)
    {
        parentRS.first = child;
        newChildRS.parent = parent;
        return;
    }

    auto firstChildRS = parentRS.first.TryGetComponent<ES::Plugin::Relationship::Component::Relationship>(core);
    if (!firstChildRS)
    {
        ES::Utils::Log::Error("Entity " + std::to_string(ES::Engine::Entity::entity_id_type(parentRS.first)) +
                              " is not a valid child of the parent " +
                              std::to_string(ES::Engine::Entity::entity_id_type(parent)));
        return;
    }
    firstChildRS->prev = child;
    newChildRS.next = parentRS.first;
    parentRS.first = child;
    newChildRS.parent = parent;
}

auto ES::Plugin::Relationship::Utils::IsChildOf(ES::Engine::Core &core, ES::Engine::Entity parent,
                                                ES::Engine::Entity child) -> bool
{
    auto &childRS = child.GetComponents<ES::Plugin::Relationship::Component::Relationship>(core);

    return childRS.parent == parent;
}
