#include "UIResource.hpp"
#include "Window.hpp"

#include <RmlUi/Core.h>

// Debugger
#include <RmlUi/Debugger.h>
// ########

namespace ES::Plugin::UI::Resource {
void UIResource::Init(ES::Engine::Core &core)
{
    _systemInterface = std::make_unique<ES::Plugin::UI::Utils::SystemInterface>();
    _renderInterface = std::make_unique<ES::Plugin::UI::Utils::RenderInterface>(core);

    Rml::SetSystemInterface(_systemInterface.get());
    Rml::SetRenderInterface(_renderInterface.get());
    Rml::Initialise();

    const auto &windowSize = core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();
    _context = Rml::CreateContext("main", Rml::Vector2i(windowSize.x, windowSize.y));
    if (!_context)
    {
        Destroy();
        throw std::runtime_error("Rmlui did not succeed upon initialization");
    }

    // Debugger
    Rml::Debugger::Initialise(_context);
}

void UIResource::Destroy()
{
    if (_document)
    {
        _document->Close();
    }

    if (_context)
    {
        Rml::RemoveContext(_context->GetName());
    }
    Rml::Debugger::Shutdown();
    Rml::Shutdown();
}

void UIResource::Render(ES::Engine::Core &core)
{
    _renderInterface.get()->BeginFrame();
    _context->Render();
    _renderInterface.get()->EndFrame(core);
}

void UIResource::Update()
{
    // if (!Rml::Debugger::IsVisible())
    //     Rml::Debugger::SetVisible(true);
    _context->Update();
}

void UIResource::SetFont(const std::string &fontPath)
{
    if (_context)
    {
        if (!Rml::LoadFontFace(fontPath))
        {
            ES::Utils::Log::Error(fmt::format("Rmlui could not load the font {}", fontPath));
        }
    }
    else
    {
        ES::Utils::Log::Warn("Rmlui font can not be assign as it has not been initialized");
    }
}

void UIResource::InitDocument(const std::string &docPath)
{
    _document = _context->LoadDocument(docPath);
    if (!_document)
    {
        Destroy();
        throw std::runtime_error("Rmlui did not succeed reading document");
    }

    _document->Show();
}

void UIResource::UpdateInnerContent(const std::string &childId, const std::string &content)
{
    Rml::Element *element = _document->GetElementById(childId.c_str());
    if (element)
    {
        if (element->GetInnerRML() != content)
            element->SetInnerRML(content);
    }
    else
    {
        ES::Utils::Log::Warn(fmt::format("RmlUi: Could not update node id '{}' with '{}': Not found", childId, content));
    }
}

} // namespace ES::Plugin::UI::Resource
