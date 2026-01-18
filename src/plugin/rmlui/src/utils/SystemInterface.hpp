#pragma once

#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Log.h"
#include "RmlUi/Core/SystemInterface.h"
#include <chrono>
#include <string>

#include <RmlUi/Core.h>

namespace Rmlui::Utils {
class SystemInterface : public Rml::SystemInterface {
  public:
    SystemInterface();
    ~SystemInterface() override = default;

    double GetElapsedTime() override;
    bool LogMessage(Rml::Log::Type type, const Rml::String &message) override;
    void SetClipboardText(const Rml::String &text) override;
    void GetClipboardText(Rml::String &text) override;

  private:
    std::chrono::steady_clock::time_point _startTime;
    std::string _clipboard;
};
} // namespace Rmlui::Utils
