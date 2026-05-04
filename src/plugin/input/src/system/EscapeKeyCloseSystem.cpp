#include "system/EscapeKeyCloseSystem.hpp"
#include "core/Core.hpp"
#include "resource/InputManager.hpp"

static void EscapeKeyCloseCallback(Engine::Core &core, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        core.Stop();
    }
}

void Input::System::EscapeKeyCloseSystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    inputManager.RegisterKeyCallback(EscapeKeyCloseCallback);
}
