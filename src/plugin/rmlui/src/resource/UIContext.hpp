#pragma once

#include <RmlUi/Core.h>
#include <RmlUi/Core/EventListener.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "AUIContext.hpp"
#include "FunctionID.hpp"
#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Context.h"
#include "RmlUi/Core/ElementDocument.h"
#include "core/Core.hpp"

namespace Rmlui::Resource {
class UIContext : public AUIContext {
  public:
    struct InputCallbackIds {
        std::optional<FunctionUtils::FunctionID> keyCallbackId;
        std::optional<FunctionUtils::FunctionID> charCallbackId;
        std::optional<FunctionUtils::FunctionID> mouseButtonCallbackId;
        std::optional<FunctionUtils::FunctionID> cursorPosCallbackId;
        std::optional<FunctionUtils::FunctionID> scrollCallbackId;
    };

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
    bool LoadOverlayDocument(const std::string &docPath);
    bool UnloadOverlayDocument(const std::string &docPath);
    const std::string &GetTitle() const override;
    Rml::ElementDocument *GetDocument();
    Rml::ElementDocument *GetOverlayDocument(const std::string &docPath);
    const Rml::ElementDocument *GetDocument() const;
    const Rml::ElementDocument *GetOverlayDocument(const std::string &docPath) const;
    void EnableDebugger(bool enable);
    Rml::Element *GetElementById(const std::string &elementId);
    bool RegisterEventListener(Rml::Element &element, const Rml::String &eventType,
                               std::function<void(Rml::Event &)> callback, bool useCapture = false);
    bool UnregisterEventListener(Rml::Element &element, const Rml::String &eventType);
    void SetInputCallbackIds(const InputCallbackIds &ids);
    bool AreInputCallbacksRegistered() const;
    void SetInputCallbacksRegistered(bool registered);

    bool ProcessKey(int key, int action, int mods) override;
    bool ProcessText(unsigned int codepoint) override;
    bool ProcessMouseMove(double x, double y, int mods) override;
    bool ProcessMouseButton(int button, int action, int mods) override;
    bool ProcessMouseWheel(double xoffset, double yoffset, int mods) override;

  protected:
    void _setup(Engine::Core &core) override;

  private:
    struct EventListenerEntry {
        Rml::Element *element = nullptr;
        Rml::String eventType;
        bool useCapture = false;
        std::unique_ptr<Rml::EventListener> listener;
    };

    Rml::Context *_context = nullptr;
    Rml::ElementDocument *_document = nullptr;
    std::unordered_map<std::string, Rml::ElementDocument *> _overlayDocuments;
    std::string _titleCache;
    bool _debuggerInitialized = false;
    std::vector<EventListenerEntry> _eventListeners;
    InputCallbackIds _inputCallbackIds;
    bool _inputCallbacksRegistered = false;

    bool _isReady() const;
};
} // namespace Rmlui::Resource
