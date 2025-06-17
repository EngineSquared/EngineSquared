#include "UIResource.hpp"
#include "Window.hpp"

#include <RmlUi/Core.h>

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
    _context->SetDimensions(Rml::Vector2i(windowSize.x, windowSize.y));
    _event = std::make_unique<ES::Plugin::UI::Utils::RmlEventBackend>(core, *_context);
}

void UIResource::BindEventCallback()
{
    _event->SetCallback();
}

void UIResource::UpdateMouseMoveEvent(ES::Engine::Core &core)
{
    const auto &window = core.GetResource<ES::Plugin::Window::Resource::Window>().GetGLFWWindow();
    double x, y;

    glfwGetCursorPos(window, &x, &y);
    _context->ProcessMouseMove(static_cast<int>(x), static_cast<int>(y), 0);
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
    Rml::Shutdown();
}

void UIResource::Render()
{
    _renderInterface.get()->BeginFrame();
    _context->Render();
    _renderInterface.get()->EndFrame();
}

void UIResource::Update(ES::Engine::Core &core)
{
    const auto &windowSize = core.GetResource<ES::Plugin::Window::Resource::Window>().GetSize();
    
    _context->SetDimensions(Rml::Vector2i(windowSize.x, windowSize.y));
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
    _document->SetProperty("width", "100%");
    _document->SetProperty("height", "100%");
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
        ES::Utils::Log::Warn(
            fmt::format("RmlUi: Could not update node id '{}' with '{}': Not found", childId, content));
    }
}

void UIResource::SetTransformProperty(const std::string &childId, const std::vector<TransformParam> &transforms)
{
    std::vector<Rml::TransformPrimitive> rmlTransforms;

    for (const auto &t : transforms)
    {
        switch (t.type)
        {
            case TransformType::Rotate:
                rmlTransforms.push_back(Rml::Transforms::Rotate2D{t.value});
                break;
            case TransformType::TranslateX:
                rmlTransforms.push_back(Rml::Transforms::TranslateX{t.value});
                break;
            case TransformType::TranslateY:
                rmlTransforms.push_back(Rml::Transforms::TranslateY{t.value});
                break;
        }
    }

    auto property = Rml::Transform::MakeProperty(rmlTransforms);

    Rml::Element *element = _document->GetElementById(childId.c_str());
    if (element)
    {
        element->SetProperty(Rml::PropertyId::Transform, property);
    }
    else
    {
        ES::Utils::Log::Warn(fmt::format("RmlUi: Could not apply property to node id '{}': Not found", childId));
    }
}

void UIResource::AttachEventHandlers(const std::string &elementId, const std::string &eventType, ES::Plugin::UI::Utils::RmlEventBackend::EventCallback callback)
{
    Rml::Element *element = _document->GetElementById(elementId.c_str());

    if (!element)
    {
        ES::Utils::Log::Error(fmt::format("RmlUi: Could not attach events to sub elements of {}: Not found.", elementId.c_str()));
        return;
    }
    _event->SetEventCallback(callback);
    _event->AttachEvents(eventType, *element);
}

} // namespace ES::Plugin::UI::Resource
