#pragma once

#include <RmlUi/Core.h>

#include <string>

#include "AUIContext.hpp"
#include "RmlUi/Core/Context.h"
#include "core/Core.hpp"

namespace Rmlui::Resource {
class UIContext : public AUIContext {
  public:
    UIContext() = default;
    ~UIContext() override = default;

    UIContext(const UIContext &) = delete;
    UIContext &operator=(const UIContext &) = delete;
    UIContext(UIContext &&) noexcept = default;
    UIContext &operator=(UIContext &&) noexcept = default;

    void Update(Engine::Core &core) override;
    void Render(Engine::Core &core) override;
    void Destroy(Engine::Core &core) override;
    void UpdateMouseMoveEvent(Engine::Core &core) override;

    void SetFont(const std::string &fontPath) override;
    void LoadDocument(const std::string &docPath) override;
    const std::string &GetTitle() const override;
    void EnableDebugger(bool enable);

    bool ProcessKey(int key, int action, int mods) override;
    bool ProcessText(unsigned int codepoint) override;
    bool ProcessMouseMove(double x, double y, int mods) override;
    bool ProcessMouseButton(int button, int action, int mods) override;
    bool ProcessMouseWheel(double xoffset, double yoffset, int mods) override;

  protected:
    void _setup(Engine::Core &core) override;

  private:
    Rml::Context *_context = nullptr;
    Rml::ElementDocument *_document = nullptr;
    std::string _titleCache;
    bool _debuggerInitialized = false;

    bool _isReady() const;
};
} // namespace Rmlui::Resource
