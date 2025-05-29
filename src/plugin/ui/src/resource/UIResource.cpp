#include "UIResource.hpp"
#include "Window.hpp"

#include <RmlUi/Core.h>

// Debugger
#include <RmlUi/Debugger.h>
// ########

namespace ES::Plugin::UI::Resource {
void UIResource::Init(ES::Engine::Core &core)
{
    std::cout << "init" << std::endl;
    _systemInterface = std::make_unique<ES::Plugin::UI::Utils::SystemInterface>();
    _renderInterface = std::make_unique<ES::Plugin::UI::Utils::RenderInterface>(core);

    Rml::SetSystemInterface(_systemInterface.get());
    Rml::SetRenderInterface(_renderInterface.get());
    Rml::Initialise();

    const auto &windowSize = core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();
    // _context.reset(Rml::CreateContext("main", Rml::Vector2i(windowSize.x, windowSize.y)));
    _context = Rml::CreateContext("main", Rml::Vector2i(windowSize.x, windowSize.y));
    // if (!_context || !_context.get())
    if (!_context)
    {
        Destroy();
        throw std::runtime_error("Rmlui did not succeed upon initialization");
    }

    // Debugger
    // Rml::Debugger::Initialise(_context.get());
    Rml::Debugger::Initialise(_context);
}

void UIResource::Destroy()
{
    std::cout << "destroy" << std::endl;
    if (_document)
    {
        _document->Close();
        // _document.reset();
    }

    if (_context)
    {
        Rml::RemoveContext(_context->GetName());
        // _context.reset();
    }
    Rml::Debugger::Shutdown();
    Rml::Shutdown();
}

void UIResource::Render(ES::Engine::Core &core)
{
    _context->Render();
}

void UIResource::Update() { _context->Update(); }

void UIResource::SetFont(const std::string &fontPath)
{
    std::cout << "font set" << std::endl;
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
    std::cout << "document" << std::endl;
    // _document.reset(_context->LoadDocument(docPath));
    _document = _context->LoadDocument(docPath);
    // if (!_document || !_document.get())
    if (!_document)
    {
        Destroy();
        throw std::runtime_error("Rmlui did not succeed reading document");
    }

    _document->Show();
}
} // namespace ES::Plugin::UI::Resource
