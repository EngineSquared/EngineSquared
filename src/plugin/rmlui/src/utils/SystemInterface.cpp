#include "utils/SystemInterface.hpp"

#include "Logger.hpp"
#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Log.h"
#include <chrono>

namespace Rmlui::Utils {
SystemInterface::SystemInterface() = default;

double SystemInterface::GetElapsedTime()
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - _startTime).count();
}

bool SystemInterface::LogMessage(Rml::Log::Type type, const Rml::String &message)
{
    switch (type)
    {
    case Rml::Log::Type::LT_INFO: Log::Info(message); break;
    case Rml::Log::Type::LT_WARNING: Log::Warn(message); break;
    case Rml::Log::Type::LT_ERROR: Log::Error(message); break;
    case Rml::Log::Type::LT_ASSERT: Log::Critical(message); break;
    case Rml::Log::Type::LT_DEBUG: Log::Debug(message); break;
    default: Log::Info(message); break;
    }
    return true;
}

void SystemInterface::SetClipboardText(const Rml::String &text) { _clipboard = text; }

void SystemInterface::GetClipboardText(Rml::String &text) { text = _clipboard; }
} // namespace Rmlui::Utils
