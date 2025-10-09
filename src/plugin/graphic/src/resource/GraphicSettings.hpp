#pragma once

namespace Plugin::Graphic::Resource {

enum class WindowSystem {
    None,
    GLFW
};

class GraphicSettings {
    public:
        GraphicSettings() = default;
        ~GraphicSettings() = default;

        WindowSystem GetWindowSystem() const { return windowSystem; }
        void SetWindowSystem(WindowSystem system) { windowSystem = system; }

    private:
        WindowSystem windowSystem = WindowSystem::GLFW;
};
} // namespace Plugin::Graphic::Resource
