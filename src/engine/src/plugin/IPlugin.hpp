#pragma once

namespace Engine {

enum class PluginState {
    JustAdded,
    StartingUp,
    Running,
    ShuttingDown,
    ShuttedDown,
};

/// @class IPlugin
/// @brief Interface for plugins that can be added to the engine.
class IPlugin {
  public:
    /// @brief Virtual destructor for IPlugin.
    virtual ~IPlugin() = default;
    virtual void Bind(void) = 0;

    virtual PluginState GetState() const = 0;

    virtual void EmitStateFinished(PluginState state) = 0;

    virtual void Attach(void) = 0;
    virtual void Unbind(void) = 0;
};
} // namespace Engine
