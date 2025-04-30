#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <functional>
#include <vector>

#include "CallableFunction.hpp"
#include "Core.hpp"
#include "FunctionContainer.hpp"

namespace ES::Plugin::Input::Resource {
/**
 * InputManager is a singleton class that wraps the GLFW callback functions.
 */
class InputManager {
  public:
    InputManager()
    {
        _keyCallbacks = std::make_shared<KeyCallbackContainer>();
        _charCallbacks = std::make_shared<CharCallbackContainer>();
        _charModsCallbacks = std::make_shared<CharModsCallbackContainer>();
        _mouseButtonCallbacks = std::make_shared<MouseButtonCallbackContainer>();
        _cursorPosCallbacks = std::make_shared<CursorPosCallbackContainer>();
        _cursorEnterCallbacks = std::make_shared<CursorEnterCallbackContainer>();
        _scrollCallbacks = std::make_shared<ScrollCallbackContainer>();
        _dropCallbacks = std::make_shared<DropCallbackContainer>();
    }
    ~InputManager() = default;

    /**
     * @brief Register a key callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a key is pressed or released.
     */
    template <typename TCallable> inline void RegisterKeyCallback(TCallable callback)
    {
        using KeyCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, int, int, int, int>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, int, int, int, int>>
            keyCallback = std::make_unique<KeyCallback>(callback);

        _keyCallbacks->AddFunction(std::move(keyCallback));
    }

    /**
     * @brief Register a char callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a character is typed.
     */
    template <typename TCallable> inline void RegisterCharCallback(TCallable callback)
    {
        using CharCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, unsigned int>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, unsigned int>>
            charCallback = std::make_unique<CharCallback>(callback);

        _charCallbacks->AddFunction(std::move(charCallback));
    }

    /**
     * @brief Register a char mods callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a character is typed with modifiers.
     */
    template <typename TCallable> inline void RegisterCharModsCallback(TCallable callback)
    {
        using CharModsCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, unsigned int, int>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, unsigned int, int>>
            charModsCallback = std::make_unique<CharModsCallback>(callback);

        _charModsCallbacks->AddFunction(std::move(charModsCallback));
    }

    /**
     * @brief Register a mouse button callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a mouse button is pressed or released.
     */
    template <typename TCallable> inline void RegisterMouseButtonCallback(TCallable callback)
    {
        using MouseButtonCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, int, int, int>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, int, int, int>>
            mouseButtonCallback = std::make_unique<MouseButtonCallback>(callback);

        _mouseButtonCallbacks->AddFunction(std::move(mouseButtonCallback));
    }

    /**
     * @brief Register a cursor position callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the cursor position changes.
     */
    template <typename TCallable> inline void RegisterCursorPosCallback(TCallable callback)
    {
        using CursorPosCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, double, double>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, double, double>>
            cursorPosCallback = std::make_unique<CursorPosCallback>(callback);

        _cursorPosCallbacks->AddFunction(std::move(cursorPosCallback));
    }

    /**
     * @brief Register a cursor enter callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the cursor enters or leaves the window.
     */
    template <typename TCallable> inline void RegisterCursorEnterCallback(TCallable callback)
    {
        using CursorEnterCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, int>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, int>> cursorEnterCallback =
            std::make_unique<CursorEnterCallback>(callback);

        _cursorEnterCallbacks->AddFunction(std::move(cursorEnterCallback));
    }

    /**
     * @brief Register a scroll callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when the mouse wheel is scrolled.
     */
    template <typename TCallable> inline void RegisterScrollCallback(TCallable callback)
    {
        using ScrollCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, double, double>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, double, double>>
            scrollCallback = std::make_unique<ScrollCallback>(callback);

        _scrollCallbacks->AddFunction(std::move(scrollCallback));
    }

    /**
     * @brief Register a drop callback.
     *
     * @param callback The callback to register.
     *
     * @note The callback will be called when a file is dropped on the window.
     */
    template <typename TCallable> inline void RegisterDropCallback(TCallable callback)
    {
        using DropCallback =
            ES::Utils::FunctionContainer::CallableFunction<TCallable, void, ES::Engine::Core &, int, const char **>;
        std::unique_ptr<ES::Utils::FunctionContainer::BaseFunction<void, ES::Engine::Core &, int, const char **>>
            dropCallback = std::make_unique<DropCallback>(callback);

        _dropCallbacks->AddFunction(std::move(dropCallback));
    }

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
        for (auto &callback : _keyCallbacks->GetFunctions())
        {
            callback->Call(core, key, scancode, action, mods);
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
        for (auto &callback : _charCallbacks->GetFunctions())
        {
            callback->Call(core, codepoint);
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
        for (auto &callback : _charModsCallbacks->GetFunctions())
        {
            callback->Call(core, codepoint, mods);
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
        for (auto &callback : _mouseButtonCallbacks->GetFunctions())
        {
            callback->Call(core, button, action, mods);
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
        for (auto &callback : _cursorPosCallbacks->GetFunctions())
        {
            callback->Call(core, xpos, ypos);
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
        for (auto &callback : _cursorEnterCallbacks->GetFunctions())
        {
            callback->Call(core, entered);
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
        for (auto &callback : _scrollCallbacks->GetFunctions())
        {
            callback->Call(core, xoffset, yoffset);
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
        for (auto &callback : _dropCallbacks->GetFunctions())
        {
            callback->Call(core, count, paths);
        }
    }

    /**
     * @brief Delete a key callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteKeyCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _keyCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a char callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteCharCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _charCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a char mods callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteCharModsCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _charModsCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a mouse button callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteMouseButtonCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _mouseButtonCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a cursor position callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteCursorPosCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _cursorPosCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a cursor enter callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteCursorEnterCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _cursorEnterCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a scroll callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteScrollCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _scrollCallbacks->DeleteFunction(id);
    }

    /**
     * @brief Delete a drop callback.
     *
     * @param id The ID of the callback to delete.
     * @return True if the callback was deleted, false otherwise.
     */
    inline bool DeleteDropCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _dropCallbacks->DeleteFunction(id);
    }

  private:
    using KeyCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, int, int, int, int>;
    std::shared_ptr<KeyCallbackContainer> _keyCallbacks;
    using CharCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, unsigned int>;
    std::shared_ptr<CharCallbackContainer> _charCallbacks;
    using CharModsCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, unsigned int, int>;
    std::shared_ptr<CharModsCallbackContainer> _charModsCallbacks;
    using MouseButtonCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, int, int, int>;
    std::shared_ptr<MouseButtonCallbackContainer> _mouseButtonCallbacks;
    using CursorPosCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, double, double>;
    std::shared_ptr<CursorPosCallbackContainer> _cursorPosCallbacks;
    using CursorEnterCallbackContainer = ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, int>;
    std::shared_ptr<CursorEnterCallbackContainer> _cursorEnterCallbacks;
    using ScrollCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, double, double>;
    std::shared_ptr<ScrollCallbackContainer> _scrollCallbacks;
    using DropCallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, int, const char **>;
    std::shared_ptr<DropCallbackContainer> _dropCallbacks;
};
} // namespace ES::Plugin::Input::Resource
