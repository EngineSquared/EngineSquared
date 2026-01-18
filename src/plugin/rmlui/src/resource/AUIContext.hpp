#pragma once

#include <concepts>
#include <memory>
#include <string>
#include <type_traits>

#include <RmlUi/Core.h>

#include "RmlUi/Core/RenderInterface.h"
#include "RmlUi/Core/SystemInterface.h"
#include "core/Core.hpp"
#include "utils/IRenderer.hpp"

template <typename T>
concept CSystemInterface = std::is_base_of_v<Rml::SystemInterface, T>;

template <typename T>
concept CRenderInterface = std::is_base_of_v<Rml::RenderInterface, T> && requires(Engine::Core &core) { T(core); };

namespace Rmlui::Resource {
class AUIContext {
  public:
    template <CSystemInterface TSystemInterface, CRenderInterface TRenderInterface> void Init(Engine::Core &core)
    {
        _systemInterface = std::make_unique<TSystemInterface>();
        _renderInterface = std::make_unique<TRenderInterface>(core);
        _setup(core);
    }

    virtual ~AUIContext() = default;
    AUIContext() = default;

    AUIContext(const AUIContext &) = delete;
    AUIContext &operator=(const AUIContext &) = delete;
    AUIContext(AUIContext &&) noexcept = default;
    AUIContext &operator=(AUIContext &&) noexcept = default;

    virtual void UpdateMouseMoveEvent(Engine::Core &core) = 0;
    virtual void Destroy(Engine::Core &core) = 0;
    virtual void Update(Engine::Core &core) = 0;
    virtual void Render(Engine::Core &core) = 0;

    virtual void SetFont(const std::string &fontPath) = 0;
    virtual void LoadDocument(const std::string &docPath) = 0;
    [[nodiscard]] virtual const std::string &GetTitle() const = 0;

    virtual bool ProcessKey(int key, int action, int mods) = 0;
    virtual bool ProcessText(unsigned int codepoint) = 0;
    virtual bool ProcessMouseMove(double x, double y, int mods) = 0;
    virtual bool ProcessMouseButton(int button, int action, int mods) = 0;
    virtual bool ProcessMouseWheel(double xoffset, double yoffset, int mods) = 0;

  protected:
    Rml::SystemInterface *GetSystemInterface() { return _systemInterface.get(); }
    const Rml::SystemInterface *GetSystemInterface() const { return _systemInterface.get(); }
    Rmlui::Utils::IRenderer *GetRenderInterface() { return _renderInterface.get(); }
    const Rmlui::Utils::IRenderer *GetRenderInterface() const { return _renderInterface.get(); }

    virtual void _setup(Engine::Core &core) = 0;

  private:
    std::unique_ptr<Rml::SystemInterface> _systemInterface;
    std::unique_ptr<Rmlui::Utils::IRenderer> _renderInterface;
};
} // namespace Rmlui::Resource
