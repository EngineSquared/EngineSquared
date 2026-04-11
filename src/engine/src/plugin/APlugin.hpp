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
    template <CScheduler TScheduler, typename... Systems> decltype(auto) RegisterSystems(Systems... systems);

    /// @brief Register a resource in the core.
    /// @tparam TResource The type of the resource to register.
    /// @param resource The resource to register.
    /// @return A reference to the registered resource.
    /// @see Engine::Core::RegisterResource
    template <typename TResource> TResource &RegisterResource(TResource &&resource);

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

  private:
    /// @brief Add a plugin to the core if it is not already added.
    /// @tparam TPlugin The type of the plugin to add.
    /// @see Engine::APlugin::RequirePlugins
    /// @see Engine::Core::HasPlugin
    /// @see Engine::Core::AddPlugins
    template <CPlugin TPlugin> void RequirePlugin();

    /// @brief Reference to the core, which is used to register systems and resources in the Bind method.
    Core &_core;
};
} // namespace Engine

#include "plugin/APlugin.ipp"
