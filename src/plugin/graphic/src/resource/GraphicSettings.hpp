#pragma once

namespace Plugin::Graphic::Resource {

enum class WindowSystem {
    None,
    GLFW
};

enum class PowerPreference {
    LowPower,
    HighPerformance
};

class GraphicSettings {
  public:
    GraphicSettings() = default;
    ~GraphicSettings() = default;

    WindowSystem GetWindowSystem() const { return windowSystem; }
    void SetWindowSystem(WindowSystem system) { windowSystem = system; }

    PowerPreference GetPowerPreference() const { return powerPreference; }
    void SetPowerPreference(PowerPreference preference) { powerPreference = preference; }

  private:
    WindowSystem windowSystem = WindowSystem::GLFW;
    PowerPreference powerPreference = PowerPreference::HighPerformance;
};
} // namespace Plugin::Graphic::Resource
