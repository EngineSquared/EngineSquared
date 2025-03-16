#pragma once

namespace ES::Engine
{
    class Core;

    class APlugin {
    public:
        APlugin(Core &core) : _core(core) {};
        virtual void Build(void) = 0;

        // AddSystem
        template <typename TScheduler, typename... Systems>
        void AddSystems(Systems... systems)
        {
            _core.RegisterSystem<TScheduler>(systems...);
        }

        template <typename TResource>
        TResource &RegisterResource(TResource &&resource)
        {
            return _core.RegisterResource(std::forward<TResource>(resource));
        }
    private:
        Core &_core;
    };
}