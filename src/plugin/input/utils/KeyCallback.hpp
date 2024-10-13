#pragma once

#include <string>
#include <functional>
#include <map>

#include "plugin/window/resource/Window.hpp"

namespace ES::Plugin::Input::Utils {
    /**
     * KeyCallback is a simple struct that holds a callback function for key events.
     */
    struct KeyCallback
    {
        std::string key;
        std::function<void()> callback;
    };
}