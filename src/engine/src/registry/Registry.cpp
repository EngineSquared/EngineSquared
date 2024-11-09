#include "Registry.hpp"

ES::Engine::Registry::Registry() : _registry(nullptr) { this->_registry = std::make_unique<entt::registry>(); }

entt::entity ES::Engine::Registry::CreateEntity() { return this->_registry->create(); }

void ES::Engine::Registry::RegisterSystem(USystem const &f) { _systems.push_back(f); }

void ES::Engine::Registry::RunSystems()
{
    for (auto &system : _systems)
    {
        system(*this);
    }
}

bool ES::Engine::Registry::IsEntityValid(entt::entity entity) { return GetRegistry().valid(entity); }

void ES::Engine::Registry::ClearEntities() { this->_registry->clear(); }
