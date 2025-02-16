#pragma once

#include "Entity.hpp"

namespace ES::Plugin::NativeScripting::Resource {
class ScriptableEntity {
    public:
        template<typename T>
        T &GetComponent()
        {
            return _entity.GetComponents<T>(*_registry);
        }

        void SetRegistry(ES::Engine::Registry *registry)
        {
            _registry = registry;
        }

        void SetEntity(ES::Engine::Entity &entity)
        {
            _entity = entity;
        }

        void SetEntity(entt::entity &entity)
        {
            _entity = ES::Engine::Entity(entity);;
        }

    private:
        ES::Engine::Entity _entity;
        ES::Engine::Registry *_registry = nullptr;
};
}