#pragma once

#include "core/Core.hpp"
#include "plugin/IPlugin.hpp"

namespace Engine {
/// @class APlugin
/// @brief Abstract plugin class that add some utility functions to make it easier to register systems and resources.
class APlugin : public IPlugin {
  public:
    /// @brief Constructor for APlugin. It takes a reference to the Core, which is used to register systems and
    ///     resources in the Bind method.
    /// @param core Reference to the Core, which is used to register systems and resources in the Bind method.
    explicit APlugin(Core &core);

    /// @brief Register systems to a scheduler.
    /// @tparam TScheduler The type of the scheduler to register the systems to.
    /// @tparam ...Systems The types of the systems to register. They should be invocable with a Core reference as the
    ///     first parameter.
    /// @param ...systems The systems to register. They should be invocable with a Core reference as the first
    ///     parameter.
    /// @return The registered systems.
    /// @see Engine::Core::RegisterSystem
    /// @see Engine::CScheduler
    template <CScheduler TScheduler, typename... Systems> void RegisterSystems(Systems... systems);

    /// @brief Register a resource in the core.
    /// @tparam TResource The type of the resource to register.
    /// @param resource The resource to register.
    /// @return A reference to the registered resource.
    /// @see Engine::Core::RegisterResource
    template <typename TResource> void RegisterResource(TResource &&resource);

    /// @brief Add a plugin to the core
    /// @tparam ...TPlugins The types of the plugins to add. They should be derived from APlugin.
    /// @see Engine::APlugin::RequirePlugin
    template <CPlugin... TPlugins> void RequirePlugins();

    /// @brief Register a scheduler in the core.
    /// @tparam ...Args The types of the arguments to pass to the scheduler constructor.
    /// @tparam TScheduler The type of the scheduler to register.
    /// @param ...args The arguments to pass to the scheduler constructor.
    /// @return A reference to the registered scheduler.
    /// @see Engine::Core::RegisterScheduler
    /// @see Engine::CScheduler
    template <CScheduler TScheduler, typename... Args> TScheduler &RegisterScheduler(Args &&...args);

    /// @brief Get a reference to the core. This can be used to register systems and resources in the Bind method.
    /// @return A reference to the core.
    /// @see Engine::Core
    Core &GetCore();

    /// Should we put this final ?
    virtual void Unbind(void) override
    {
        std::set<std::type_index> invalidPluginIds{};
        for (const auto &requiredPluginId : _requiredPluginIds)
        {
            auto requiredPluginOpt = GetCore().GetPlugin(requiredPluginId);
            if (!requiredPluginOpt.has_value())
            {
                invalidPluginIds.insert(requiredPluginId);
                continue;
            }
            requiredPluginOpt.value().get()->Unbind();
        }
        for (const auto &invalidPluginId : invalidPluginIds)
        {
            _requiredPluginIds.erase(invalidPluginId);
        }
        _unbinding = true;
    };

    virtual PluginState GetState() const override { return _state; }

    virtual void EmitStateFinished(PluginState state) override
    {
        if (_state != state)
            return;
        switch (state)
        {
        case PluginState::JustAdded:
            Attach();
            EnableSystems(SchedulerCategory::Startup);
            EnableSystems(SchedulerCategory::Runtime);
            DisableSystems(SchedulerCategory::Shutdown);
            this->_state = PluginState::StartingUp;
            break;
        case PluginState::StartingUp:
            DisableSystems(SchedulerCategory::Startup);
            EnableSystems(SchedulerCategory::Runtime);
            DisableSystems(SchedulerCategory::Shutdown);
            this->_state = PluginState::Running;
            break;
        case PluginState::Running:
            if (_unbinding)
            {
                DisableSystems(SchedulerCategory::Startup);
                DisableSystems(SchedulerCategory::Runtime);
                EnableSystems(SchedulerCategory::Shutdown);
                this->_state = PluginState::ShuttingDown;
            }
            break;
        case PluginState::ShuttingDown:
            DisableSystems(SchedulerCategory::Startup);
            DisableSystems(SchedulerCategory::Runtime);
            DisableSystems(SchedulerCategory::Shutdown);
            this->_state = PluginState::ShuttedDown;
            Detach();
            _unbinding = false;
            break;
        case PluginState::ShuttedDown:
        default:
            DisableSystems(SchedulerCategory::Startup);
            DisableSystems(SchedulerCategory::Runtime);
            DisableSystems(SchedulerCategory::Shutdown);
            if (!_registerers.empty())
            {
                this->_state = PluginState::JustAdded;
            }
        }
    }
    void Attach(void) final
    {
        for (auto &registrer : this->_registerers)
        {
            registrer(_core);
        }
        this->_registerers.clear();
    }

  private:
    void Detach(void)
    {
        std::set<FunctionUtils::FunctionID> unsyncSystems{};
        for (const auto &[category, systems] : _systems)
        {
            for (const auto &[systemId, schedulerId] : _systems.at(category))
            {
                if (GetCore().HasScheduler(schedulerId))
                    GetCore().GetScheduler(schedulerId).Remove(systemId);
                else
                {
                    Log::Warning(fmt::format("Scheduler with id {} not found in core while detaching a plugin",
                                             schedulerId.name()));
                    unsyncSystems.insert(systemId);
                }
            }
            for (const auto &systemId : unsyncSystems)
            {
                _systems.at(category).erase(systemId);
            }
        }
        _systems.clear();
        for (auto &resourceDeleter : this->_resourceDeleters)
        {
            resourceDeleter(_core);
        }
        _resourceDeleters.clear();
        _requiredPluginIds.clear();
    }

    void EnableSystems(SchedulerCategory category)
    {
        if (!_systems.contains(category))
        {
            return;
        }
        std::vector<FunctionUtils::FunctionID> unsyncSystems{};
        for (const auto &[systemId, schedulerId] : _systems.at(category))
        {
            try
            {
                GetCore().GetScheduler(schedulerId).Enable(systemId);
            }
            catch (const Exception::MissingSchedulerError &error)
            {
                Log::Warning(fmt::format("MissingSchedulerError: {}", error.what()));
                unsyncSystems.emplace_back(systemId);
            }
        }
        for (const auto &systemId : unsyncSystems)
        {
            _systems.at(category).erase(systemId);
        }
    }

    void DisableSystems(SchedulerCategory category)
    {
        if (!_systems.contains(category))
        {
            return;
        }
        for (const auto &[systemId, schedulerId] : _systems.at(category))
        {
            GetCore().GetScheduler(schedulerId).Disable(systemId);
        }
    }

    virtual void Bind(void) override {};

    template <CHasCategory TScheduler> static SchedulerCategory GetCategory(void) { return TScheduler::Category; }

    /// @brief Add a plugin to the core if it is not already added.
    /// @tparam TPlugin The type of the plugin to add.
    /// @see Engine::APlugin::RequirePlugins
    /// @see Engine::Core::HasPlugin
    /// @see Engine::Core::AddPlugins
    template <CPlugin TPlugin> void RequirePlugin();

    /// @brief Reference to the core, which is used to register systems and resources in the Bind method.
    Core &_core;

    PluginState _state = PluginState::JustAdded;

    std::unordered_map<SchedulerCategory, std::map<FunctionUtils::FunctionID, std::type_index>> _systems;
    std::list<std::function<void(Engine::Core &)>> _resourceDeleters;

    std::list<std::function<void(Engine::Core &)>> _registerers;

    bool _unbinding = false;

    std::set<std::type_index> _requiredPluginIds;

    // std::set<FunctionUtils::FunctionID> _systemIds;
};
} // namespace Engine

#include "plugin/APlugin.ipp"
