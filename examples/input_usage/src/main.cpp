#include "Engine.hpp"
#include "Input.hpp"

void Setup(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    Input::Utils::PrintAvailableControllers();

    inputManager.RegisterKeyCallback([](Engine::Core &core, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE)
            {
                core.Stop();
                return;
            }
            auto name = glfwGetKeyName(key, scancode);
            if (name == nullptr)
            {
                name = "Unknown Key";
            }
            std::cout << "Key pressed: " << name << std::endl;
        }
        else if (action == GLFW_RELEASE)
        {
            auto name = glfwGetKeyName(key, scancode);
            if (name == nullptr)
            {
                name = "Unknown Key";
            }
            std::cout << "Key released: " << name << std::endl;
        }
    });
}

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Input::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(Setup);
    core.RegisterSystem([](Engine::Core &core) {
        static std::vector<float> lastAxes;
        static std::vector<unsigned char> lastButtons;
        if (!glfwJoystickPresent(GLFW_JOYSTICK_1))
            return;
        auto axes = Input::Utils::GetJoystickAxes(GLFW_JOYSTICK_1);
        auto buttons = Input::Utils::GetJoystickButtons(GLFW_JOYSTICK_1);

        if (lastAxes.size() != axes.size())
            lastAxes.resize(axes.size(), 0.0f);
        if (lastButtons.size() != buttons.size())
            lastButtons.resize(buttons.size(), GLFW_RELEASE);

        for (size_t i = 0; i < axes.size(); ++i)
        {
            if (std::abs(axes[i] - lastAxes[i]) > 0.01f)
            {
                if (std::abs(axes[i]) > 0.1f)
                    std::cout << "Axis " << i << ": " << axes[i] << std::endl;
                lastAxes[i] = axes[i];
            }
        }
        for (size_t i = 0; i < buttons.size(); ++i)
        {
            if (buttons[i] != lastButtons[i])
            {
                std::cout << "Button " << i << ": " << (buttons[i] == GLFW_PRESS ? "Pressed" : "Released") << std::endl;
                lastButtons[i] = buttons[i];
            }
        }
    });

    core.RunCore();

    return 0;
}
