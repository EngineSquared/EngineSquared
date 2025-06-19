/*
 * The RmlUi backend is mostly taken from the official implementation of the RmlUi GLFW GL3 backend
 * See: https://github.dev/mikke89/RmlUi/blob/master/Backends/RmlUi_Renderer_GL3.h
 */

#pragma once

#include <RmlUi/Core.h>
#include <glm/gtc/type_ptr.hpp>

#include "Core.hpp"

namespace ES::Plugin::UI::Utils
{
class SystemInterface : public Rml::SystemInterface {
public:
    SystemInterface() = default;
    ~SystemInterface() = default;

    double GetElapsedTime() override;
    bool LogMessage(Rml::Log::Type type, const Rml::String &message) override;
};  
} // namespace ES::Plugin::UI::Utils
