#pragma once

namespace Engine {
/// @class IPlugin
/// @brief Interface for plugins that can be added to the engine.
class IPlugin {
  public:
    /// @brief Virtual destructor for IPlugin. This ensures that derived plugin classes can be properly cleaned up when
    ///     deleted through a pointer to IPlugin.
    virtual ~IPlugin() = default;
    /// @brief Pure virtual method that must be implemented by derived plugin classes. This method is called when the
    ///     plugin is added to the engine, and is where the plugin should register its systems, resources, and any other
    ///     functionality it provides to the engine.
    /// @see Engine::Core
    /// @see Engine::APlugin
    virtual void Bind(void) = 0;
};
} // namespace Engine
