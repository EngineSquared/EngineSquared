#pragma once

#include "Backend.hpp"
#include "Engine.hpp"

namespace ES::Plugin::UI::Resource {
class UIResource {
  private:
    Rml::Context *_context;
    Rml::ElementDocument *_document;
    std::unique_ptr<ES::Plugin::UI::Utils::SystemInterface> _systemInterface;
    std::unique_ptr<ES::Plugin::UI::Utils::RenderInterface> _renderInterface;

  public:
    /**
     * Constructor.
     */
    UIResource() = default;

    /**
     * Copy constructor should not be referenced.
     */
    UIResource(const UIResource &) = delete;
    UIResource &operator=(const UIResource &) = delete;

    /**
     * Move constructor.
     */
    UIResource(UIResource &&) noexcept = default;
    UIResource &operator=(UIResource &&) noexcept = default;

    /**
     * Destructor.
     */
    ~UIResource() = default;

    void Init(ES::Engine::Core &core);
    void Destroy();

    void Update();
    void Render();

    void SetFont(const std::string &fontPath);

    void InitDocument(const std::string &docPath);
};
} // namespace ES::Plugin::UI::Resource
