#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <functional>


namespace ES::Engine
{
    class Registry
    {
    private:
        using USystem = std::function<void(Registry&)>;
    public:
        Registry();
        ~Registry() = default;

        inline entt::registry& GetRegistry() { return *_registry; }
        entt::entity CreateEntity();

        template <typename TResource>
        inline TResource& RegisterResource(TResource resource);
        template <typename TResource>
        inline TResource& GetResource();

        void RegisterSystem(USystem const& f);
        void RunSystems();

    private:
        std::unique_ptr<entt::registry> _registry;
        std::vector<USystem> _systems;
    };
} // namespace ES::Engine

#include "Registry.inl"