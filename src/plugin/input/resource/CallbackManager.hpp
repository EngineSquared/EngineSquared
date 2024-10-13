#pragma once

#include <GLFW/glfw3.h>

#include <functional>
#include <map>

#include "Registry.hpp"
#include "plugin/window/resource/Window.hpp"

#include "../utils/CursorCallback.hpp"
#include "../utils/KeyCallback.hpp"

namespace ES::Plugin::Input::Resource {
    /**
     * CallbackManager is a singleton class that manages the callbacks for the GLFW window.
     */
    class CallbackManager
    {
        private:
            /**
             * Map that contains all the key callbacks.
             * It is a static variable to allow the callbacks to be registered in the key_callback function,
             * otherwise it won't be possible to enclose it properly.
             */
            static std::map<std::string, ES::Plugin::Input::Utils::KeyCallback> _keyCallbacks;

        public:
            CallbackManager() = default;
            ~CallbackManager() = default;

            void AddKeyCallback(ES::Plugin::Input::Utils::KeyCallback callback)
            {
                _keyCallbacks[callback.key] = callback;
            }

            void AddCursorCallback(ES::Plugin::Input::Utils::CursorCallback callback)
            {

            }

            void RegisterCallbacks(ES::Plugin::Window::Resource::Window &window)
            {
                glfwSetKeyCallback(window.GetGLFWWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
                {
                    if (action == GLFW_PRESS || action == GLFW_REPEAT)
                    {
                        const char *keyName = glfwGetKeyName(key, scancode);
                        if (keyName != nullptr && _keyCallbacks.find(keyName) != _keyCallbacks.end())
                        {
                            _keyCallbacks[keyName].callback();
                        }
                    }
                });
            }

            void UnregisterCallbacks(ES::Plugin::Window::Resource::Window &window)
            {

            }
    };

    std::map<std::string, ES::Plugin::Input::Utils::KeyCallback> CallbackManager::_keyCallbacks;
}