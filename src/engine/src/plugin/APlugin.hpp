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
    /// @todo put the implementation in the cpp file
    explicit APlugin(Core &core) : _core(core) {};

    /// @copydoc Engine::IPlugin::Bind
    virtual void Bind(void) = 0;

    /// @brief Register systems to a scheduler.
    /// @tparam TScheduler The type of the scheduler to register the systems to.
    /// @tparam ...Systems The types of the systems to register. They should be invocable with a Core reference as the
    ///     first parameter.
    /// @param ...systems The systems to register. They should be invocable with a Core reference as the first
    ///     parameter.
    /// @return The registered systems.
    /// @see Engine::Core::RegisterSystem
    /// @see Engine::CScheduler
    /// @todo put the implementation in the inl file
    template <CScheduler TScheduler, typename... Systems> inline decltype(auto) RegisterSystems(Systems... systems)
    {
        return _core.RegisterSystem<TScheduler>(systems...);
    }

    /// @brief Register a resource in the core.
    /// @tparam TResource The type of the resource to register.
    /// @param resource The resource to register.
    /// @return A reference to the registered resource.
    /// @see Engine::Core::RegisterResource
    /// @todo put the implementation in the inl file
    template <typename TResource> TResource &RegisterResource(TResource &&resource)
    {
        return _core.RegisterResource(std::forward<TResource>(resource));
    }

    /// @brief Add a plugin to the core
    /// @tparam ...TPlugins The types of the plugins to add. They should be derived from APlugin.
    /// @see Engine::APlugin::RequirePlugin
    /// @todo put the implementation in the inl file
    template <CPlugin... TPlugins> void RequirePlugins() { (RequirePlugin<TPlugins>(), ...); }

    /// @brief Register a scheduler in the core.
    /// @tparam ...Args The types of the arguments to pass to the scheduler constructor.
    /// @tparam TScheduler The type of the scheduler to register.
    /// @param ...args The arguments to pass to the scheduler constructor.
    /// @return A reference to the registered scheduler.
    /// @see Engine::Core::RegisterScheduler
    /// @see Engine::CScheduler
    /// @todo put the implementation in the inl file
    template <CScheduler TScheduler, typename... Args> inline TScheduler &RegisterScheduler(Args &&...args)
    {
        return _core.RegisterScheduler<TScheduler>(std::forward<Args>(args)...);
    }

    /// @brief Get a reference to the core. This can be used to register systems and resources in the Bind method.
    /// @return A reference to the core.
    /// @see Engine::Core
    /// @todo put the implementation in the cpp file
    Core &GetCore() { return _core; }

  private:
    /// @brief Add a plugin to the core if it is not already added.
    /// @tparam TPlugin The type of the plugin to add.
    /// @see Engine::APlugin::RequirePlugins
    /// @see Engine::Core::HasPlugin
    /// @see Engine::Core::AddPlugins
    /// @todo put the implementation in the inl file
    template <CPlugin TPlugin> void RequirePlugin()
    {
        if (!_core.HasPlugin<TPlugin>())
        {
            _core.AddPlugins<TPlugin>();
        }
    }

    /// @brief Reference to the core, which is used to register systems and resources in the Bind method.
    Core &_core;
};
} // namespace Engine
