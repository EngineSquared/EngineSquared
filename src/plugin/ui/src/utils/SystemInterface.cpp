#include "SystemInterface.hpp"

double ES::Plugin::UI::Utils::SystemInterface::GetElapsedTime()
{
    return glfwGetTime();
}

bool ES::Plugin::UI::Utils::SystemInterface::LogMessage(Rml::Log::Type type, const Rml::String &message)
{
    switch (type)
    {
    case Rml::Log::Type::LT_ASSERT:
    case Rml::Log::Type::LT_ERROR: ES::Utils::Log::Error(fmt::format("RmlUi: {}", message)); break;
    case Rml::Log::Type::LT_WARNING: ES::Utils::Log::Warn(fmt::format("RmlUi: {}", message)); break;
    case Rml::Log::Type::LT_INFO: ES::Utils::Log::Info(fmt::format("RmlUi: {}", message)); break;
    default: break;
    }
    return true;
}
