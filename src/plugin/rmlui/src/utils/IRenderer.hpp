#pragma once

#include "RmlUi/Core/RenderInterface.h"
#include <RmlUi/Core.h>

namespace Rmlui::Utils {
class IRenderer : public Rml::RenderInterface {
  public:
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
};
} // namespace Rmlui::Utils
