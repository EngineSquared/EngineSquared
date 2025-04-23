#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <functional>
#include <vector>

#include "Core.hpp"

namespace ES::Plugin::Input::Resource {
/**
 * InputManager is a singleton class that wraps the GLFW callback functions.
 */
class InputManager {
  public:
    using KeyCallbackFn = std::function<void(ES::Engine::Core &, int, int, int, int)>;
    using CharCallbackFn = std::function<void(ES::Engine::Core &, unsigned int)>;
    using CharModsCallbackFn = std::function<void(ES::Engine::Core &, unsigned int, int)>;
    using MouseButtonCallbackFn = std::function<void(ES::Engine::Core &, int, int, int)>;
    using CursorPosCallbackFn = std::function<void(ES::Engine::Core &, double, double)>;
    using CursorEnterCallbackFn = std::function<void(ES::Engine::Core &, int)>;
    using ScrollCallbackFn = std::function<void(ES::Engine::Core &, double, double)>;
    using DropCallbackFn = std::function<void(ES::Engine::Core &, int, const char **)>;
    using JoystickCallbackFn = std::function<void(int, int)>; // TODO: no window here, find a solution

    InputManager();
    ~InputManager() = default;

    /**
     * @brief Print the available controllers detected by glfw.
     *
     * @return void
     */
    void PrintAvailableControllers() const;

    /**
     * @brief Register a key callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a key is pressed or released.
     */
    inline void RegisterKeyCallback(const KeyCallbackFn &callback) { _keyCallbacks.push_back(callback); }

    /**
     * @brief Register a char callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a character is typed.
     */
    inline void RegisterCharCallback(const CharCallbackFn &callback) { _charCallbacks.push_back(callback); }

    /**
     * @brief Register a char mods callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a character is typed with modifiers.
     * (modifiers are used to detect if the key is pressed with shift, ctrl, alt, etc.)
     */
    inline void RegisterCharModsCallback(const CharModsCallbackFn &callback) { _charModsCallbacks.push_back(callback); }

    /**
     * @brief Register a mouse button callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a mouse button is pressed or released.
     */
    inline void RegisterMouseButtonCallback(const MouseButtonCallbackFn &callback)
    {
        _mouseButtonCallbacks.push_back(callback);
    }

    /**
     * @brief Register a cursor position callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the cursor position is updated.
     */
    inline void RegisterCursorPosCallback(const CursorPosCallbackFn &callback)
    {
        _cursorPosCallbacks.push_back(callback);
    }

    /**
     * @brief Register a cursor enter callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the cursor enters or leaves the window.
     */
    inline void RegisterCursorEnterCallback(const CursorEnterCallbackFn &callback)
    {
        _cursorEnterCallbacks.push_back(callback);
    }

    /**
     * @brief Register a scroll callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the scroll wheel is used.
     */
    inline void RegisterScrollCallback(const ScrollCallbackFn &callback) { _scrollCallbacks.push_back(callback); }

    /**
     * @brief Register a drop callback.
     *
     * @param callback The drop function
     *
     * @note The callback will be called when one or multiple files are dropped on the window.
     */
    inline void RegisterDropCallback(const DropCallbackFn &callback) { _dropCallbacks.push_back(callback); }

    /**
     * @brief Register a joystick callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a controller is connected or disconnected.
     */
    inline void RegisterJoystickCallback(const JoystickCallbackFn &callback) { _joystickCallbacks.push_back(callback); }

    /**
     * @brief Call the key callbacks.
     *
     * @param core The core.
     * @param key The key. (Raw input, like 'a', 'b', 'c', etc.)
     * @param scancode The scancode. (Matches with GLFW_KEY_* constants)
     * @param action The action. (One of GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT)
     * @param mods The mods. (Matches with GLFW_MOD_* constants)
     *
     * @return void
     */
    inline void CallKeyCallbacks(ES::Engine::Core &core, int key, int scancode, int action, int mods)
    {
        for (auto &callback : _keyCallbacks)
        {
            callback(core, key, scancode, action, mods);
        }
    }

    /**
     * @brief Call the char callbacks.
     *
     * @param core The core.
     * @param codepoint The codepoint. (Raw input, like 'a', 'b', 'c', etc.)
     *
     * @return void
     */
    inline void CallCharCallbacks(ES::Engine::Core &core, unsigned int codepoint)
    {
        for (auto &callback : _charCallbacks)
        {
            callback(core, codepoint);
        }
    }

    /**
     * @brief Call the char mods callbacks.
     *
     * @param core The core.
     * @param codepoint The codepoint. (Raw input, like 'a', 'b', 'c', etc.)
     * @param mods The mods. (Matches with GLFW_MOD_* constants)
     *
     * @return void
     */
    inline void CallCharModsCallbacks(ES::Engine::Core &core, unsigned int codepoint, int mods) const
    {
        for (auto &callback : _charModsCallbacks)
        {
            callback(core, codepoint, mods);
        }
    }

    /**
     * @brief Call the mouse button callbacks.
     *
     * @param core The core.
     * @param button The button. (Matches with GLFW_MOUSE_BUTTON_* constants)
     * @param action The action. (One of GLFW_PRESS, GLFW_RELEASE)
     * @param mods The mods. (Matches with GLFW_MOD_* constants)
     *
     * @return void
     */
    inline void CallMouseButtonCallbacks(ES::Engine::Core &core, int button, int action, int mods) const
    {
        for (auto &callback : _mouseButtonCallbacks)
        {
            callback(core, button, action, mods);
        }
    }

    /**
     * @brief Call the cursor position callbacks.
     *
     * @param core The core.
     * @param xpos The x position.
     * @param ypos The y position.
     *
     * @return void
     */
    inline void CallCursorPosCallbacks(ES::Engine::Core &core, double xpos, double ypos) const
    {
        for (auto &callback : _cursorPosCallbacks)
        {
            callback(core, xpos, ypos);
        }
    }

    /**
     * @brief Call the cursor enter callbacks.
     *
     * @param core The core.
     * @param entered The entered state. (1 if the cursor entered the window, 0 if it left)
     *
     * @return void
     */
    inline void CallCursorEnterCallbacks(ES::Engine::Core &core, int entered) const
    {
        for (auto &callback : _cursorEnterCallbacks)
        {
            callback(core, entered);
        }
    }

    /**
     * @brief Call the scroll callbacks.
     *
     * @param core The core.
     * @param xoffset The x offset.
     * @param yoffset The y offset.
     *
     * @return void
     */
    inline void CallScrollCallbacks(ES::Engine::Core &core, double xoffset, double yoffset) const
    {
        for (auto &callback : _scrollCallbacks)
        {
            callback(core, xoffset, yoffset);
        }
    }

    /**
     * @brief Call the drop callbacks.
     *
     * @param core The core.
     * @param count The number of paths in the C array.
     * @param paths The paths. (C array of strings)
     *
     * @return void
     */
    inline void CallDropCallbacks(ES::Engine::Core &core, int count, const char **paths) const
    {
        for (auto &callback : _dropCallbacks)
        {
            callback(core, count, paths);
        }
    }

    /**
     * @brief Call the joystick callbacks.
     *
     * @param jid The joystick id.
     * @param event The event.
     *
     * @return void
     */
    inline void CallJoystickCallbacks(int jid, int event) const
    {
        for (auto &callback : _joystickCallbacks)
        {
            callback(jid, event);
        }
    }

    // TODO: add a way to delete callbacks
  private:
    std::vector<KeyCallbackFn> _keyCallbacks;
    std::vector<CharCallbackFn> _charCallbacks;
    std::vector<CharModsCallbackFn> _charModsCallbacks;
    std::vector<MouseButtonCallbackFn> _mouseButtonCallbacks;
    std::vector<CursorPosCallbackFn> _cursorPosCallbacks;
    std::vector<CursorEnterCallbackFn> _cursorEnterCallbacks;
    std::vector<ScrollCallbackFn> _scrollCallbacks;
    std::vector<DropCallbackFn> _dropCallbacks;
    std::vector<JoystickCallbackFn> _joystickCallbacks;
};
} // namespace ES::Plugin::Input::Resource
