#include "Input.pch.hpp"

#include "InputUtils.hpp"

void Input::Utils::PrintAvailableControllers() noexcept
{
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid)
    {
        if (glfwJoystickPresent(jid))
        {
            const char *name = glfwGetJoystickName(jid);
            Log::Info(fmt::format("Detected controller {}: {}", jid, name ? name : "Unknown"));
        }
    }
}

Input::Utils::JoystickAxes Input::Utils::GetJoystickAxes(int jid)
{
    int axesCount = 0;
    const float *rawAxes = glfwGetJoystickAxes(jid, &axesCount);

    if (rawAxes == nullptr || axesCount == 0)
    {
        throw Input::InputError("Failed to get joystick axes data.");
    }

    return JoystickAxes(rawAxes, rawAxes + axesCount);
}

Input::Utils::JoystickButtons Input::Utils::GetJoystickButtons(int jid)
{
    int buttonsCount = 0;
    const unsigned char *rawButtons = glfwGetJoystickButtons(jid, &buttonsCount);

    if (rawButtons == nullptr || buttonsCount == 0)
    {
        throw Input::InputError("Failed to get joystick buttons data.");
    }

    return JoystickButtons(rawButtons, rawButtons + buttonsCount);
}
