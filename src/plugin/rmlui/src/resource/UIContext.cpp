#include "resource/UIContext.hpp"
#include "resource/Window.hpp"

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include "Logger.hpp"
#include "core/Core.hpp"
#include "fmt/format.h"
#include "spdlog/fmt/bundled/format.h"

#include "RmlUi/Config/Config.h"
#include "RmlUi/Debugger/Debugger.h"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/Input.h"
#include "RmlUi/Core/Math.h"
#include "RmlUi/Core/Types.h"
#include "RmlUi/Core/Event.h"
#include "RmlUi/Core/EventListener.h"

#include "exception/CreateRmlContextError.hpp"
#include "exception/ReadRmlDocumentError.hpp"

namespace Rmlui::Resource {
namespace {
class CallbackEventListener final : public Rml::EventListener {
  public:
    explicit CallbackEventListener(std::function<void(Rml::Event &)> callback) : _callback(std::move(callback)) {}

    void ProcessEvent(Rml::Event &event) override
    {
        if (_callback)
        {
            _callback(event);
        }
    }

  private:
    std::function<void(Rml::Event &)> _callback;
};

Rml::Input::KeyIdentifier ToRmlKey(int key)
{
    using KI = Rml::Input::KeyIdentifier;

    if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    {
        return static_cast<KI>(static_cast<int>(KI::KI_A) + (key - GLFW_KEY_A));
    }
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
    {
        return static_cast<KI>(static_cast<int>(KI::KI_0) + (key - GLFW_KEY_0));
    }
    if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F24)
    {
        return static_cast<KI>(static_cast<int>(KI::KI_F1) + (key - GLFW_KEY_F1));
    }
    if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9)
    {
        return static_cast<KI>(static_cast<int>(KI::KI_NUMPAD0) + (key - GLFW_KEY_KP_0));
    }

    switch (key)
    {
    case GLFW_KEY_SPACE: return KI::KI_SPACE;
    case GLFW_KEY_APOSTROPHE: return KI::KI_OEM_7;
    case GLFW_KEY_COMMA: return KI::KI_OEM_COMMA;
    case GLFW_KEY_MINUS: return KI::KI_OEM_MINUS;
    case GLFW_KEY_PERIOD: return KI::KI_OEM_PERIOD;
    case GLFW_KEY_SLASH: return KI::KI_OEM_2;
    case GLFW_KEY_SEMICOLON: return KI::KI_OEM_1;
    case GLFW_KEY_EQUAL: return KI::KI_OEM_PLUS;
    case GLFW_KEY_LEFT_BRACKET: return KI::KI_OEM_4;
    case GLFW_KEY_BACKSLASH: return KI::KI_OEM_5;
    case GLFW_KEY_RIGHT_BRACKET: return KI::KI_OEM_6;
    case GLFW_KEY_GRAVE_ACCENT: return KI::KI_OEM_3;
    case GLFW_KEY_ENTER: return KI::KI_RETURN;
    case GLFW_KEY_ESCAPE: return KI::KI_ESCAPE;
    case GLFW_KEY_BACKSPACE: return KI::KI_BACK;
    case GLFW_KEY_TAB: return KI::KI_TAB;
    case GLFW_KEY_INSERT: return KI::KI_INSERT;
    case GLFW_KEY_DELETE: return KI::KI_DELETE;
    case GLFW_KEY_RIGHT: return KI::KI_RIGHT;
    case GLFW_KEY_LEFT: return KI::KI_LEFT;
    case GLFW_KEY_DOWN: return KI::KI_DOWN;
    case GLFW_KEY_UP: return KI::KI_UP;
    case GLFW_KEY_PAGE_UP: return KI::KI_PRIOR;
    case GLFW_KEY_PAGE_DOWN: return KI::KI_NEXT;
    case GLFW_KEY_HOME: return KI::KI_HOME;
    case GLFW_KEY_END: return KI::KI_END;
    case GLFW_KEY_CAPS_LOCK: return KI::KI_CAPITAL;
    case GLFW_KEY_SCROLL_LOCK: return KI::KI_SCROLL;
    case GLFW_KEY_NUM_LOCK: return KI::KI_NUMLOCK;
    case GLFW_KEY_PRINT_SCREEN: return KI::KI_SNAPSHOT;
    case GLFW_KEY_PAUSE: return KI::KI_PAUSE;
    case GLFW_KEY_KP_DECIMAL: return KI::KI_DECIMAL;
    case GLFW_KEY_KP_DIVIDE: return KI::KI_DIVIDE;
    case GLFW_KEY_KP_MULTIPLY: return KI::KI_MULTIPLY;
    case GLFW_KEY_KP_SUBTRACT: return KI::KI_SUBTRACT;
    case GLFW_KEY_KP_ADD: return KI::KI_ADD;
    case GLFW_KEY_KP_ENTER: return KI::KI_NUMPADENTER;
    case GLFW_KEY_LEFT_SHIFT: return KI::KI_LSHIFT;
    case GLFW_KEY_RIGHT_SHIFT: return KI::KI_RSHIFT;
    case GLFW_KEY_LEFT_CONTROL: return KI::KI_LCONTROL;
    case GLFW_KEY_RIGHT_CONTROL: return KI::KI_RCONTROL;
    case GLFW_KEY_LEFT_ALT: return KI::KI_LMENU;
    case GLFW_KEY_RIGHT_ALT: return KI::KI_RMENU;
    case GLFW_KEY_LEFT_SUPER: return KI::KI_LWIN;
    case GLFW_KEY_RIGHT_SUPER: return KI::KI_RWIN;
    default: return KI::KI_UNKNOWN;
    }
}

int ToRmlModifiers(int mods)
{
    int modifiers = 0;
    if ((mods & GLFW_MOD_SHIFT) != 0)
    {
        modifiers |= Rml::Input::KM_SHIFT;
    }
    if ((mods & GLFW_MOD_CONTROL) != 0)
    {
        modifiers |= Rml::Input::KM_CTRL;
    }
    if ((mods & GLFW_MOD_ALT) != 0)
    {
        modifiers |= Rml::Input::KM_ALT;
    }
    if ((mods & GLFW_MOD_SUPER) != 0)
    {
        modifiers |= Rml::Input::KM_META;
    }
    if ((mods & GLFW_MOD_CAPS_LOCK) != 0)
    {
        modifiers |= Rml::Input::KM_CAPSLOCK;
    }
    if ((mods & GLFW_MOD_NUM_LOCK) != 0)
    {
        modifiers |= Rml::Input::KM_NUMLOCK;
    }
    return modifiers;
}

int ToRmlMouseButtonIndex(int button)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT: return 0;
    case GLFW_MOUSE_BUTTON_RIGHT: return 1;
    case GLFW_MOUSE_BUTTON_MIDDLE: return 2;
    case GLFW_MOUSE_BUTTON_4: return 3;
    case GLFW_MOUSE_BUTTON_5: return 4;
    default: return 0;
    }
}
} // namespace

void UIContext::_setup(Engine::Core &core)
{
    Rml::SetSystemInterface(GetSystemInterface());
    Rml::SetRenderInterface(GetRenderInterface());
    Rml::Initialise();

    const auto &windowSize = core.GetResource<Window::Resource::Window>().GetSize();
    _context =
        Rml::CreateContext("main", Rml::Vector2i(static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)));
    if (_context == nullptr)
    {
        Destroy(core);
        throw Rmlui::Exception::CreateRmlContextError("Failed to create Rml::Context");
    }
    _context->SetDimensions(Rml::Vector2i(static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)));
}

void UIContext::Destroy(Engine::Core &core)
{
    (void) core; // TODO: Might not be needed in the context

    if (_document != nullptr)
    {
        _document->Close();
        _document = nullptr;
    }
    if (_context != nullptr)
    {
        Rml::RemoveContext(_context->GetName());
        _context = nullptr;
    }
    _titleCache.clear();
    Rml::Shutdown();
    _eventListeners.clear();
}

void UIContext::Update(Engine::Core &core)
{
    if (!_isReady())
    {
        return;
    }

    const auto &windowSize = core.GetResource<Window::Resource::Window>().GetSize();
    _context->SetDimensions(Rml::Vector2i(static_cast<int>(windowSize.x), static_cast<int>(windowSize.y)));
    _context->Update();
}

void UIContext::Render(Engine::Core & /*core*/)
{
    if (!_isReady())
    {
        Log::Warn("Rmlui is not ready to render");
        return;
    }

    GetRenderInterface()->BeginFrame();
    _context->Render();
    GetRenderInterface()->EndFrame();
}

void UIContext::UpdateMouseMoveEvent(Engine::Core &core)
{
    if (_context == nullptr)
    {
        return;
    }

    const auto &window = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
    double x = 0.0;
    double y = 0.0;

    glfwGetCursorPos(window, &x, &y);
    _context->ProcessMouseMove(static_cast<int>(x), static_cast<int>(y), 0);
}

bool UIContext::_isReady() const
{
    return (_context != nullptr && _document != nullptr && GetSystemInterface() != nullptr &&
            GetRenderInterface() != nullptr);
}

void UIContext::SetFont(const std::string &fontPath)
{
    if (_context != nullptr)
    {
        if (!Rml::LoadFontFace(fontPath))
        {
            Log::Error(fmt::format("Rmlui could not load the font {}", fontPath));
        }
    }
    else
    {
        Log::Warn("Rmlui font can not be assign as it has not been initialized");
    }
}

void UIContext::LoadDocument(const std::string &docPath)
{
    if (_context == nullptr)
    {
        return;
    }

    if (_document != nullptr)
    {
        _document->Close();
        _document = nullptr;
    }

    _context->UnloadAllDocuments();
    _document = _context->LoadDocument(docPath);
    if (_document == nullptr)
    {
        throw Rmlui::Exception::ReadRmlDocumentError("Rmlui did not succeed reading document");
    }

    _document->Show();
    _document->SetProperty("width", "100%");
    _document->SetProperty("height", "100%");
    _titleCache = _document->GetTitle();
}

const std::string &UIContext::GetTitle() const { return _titleCache; }

void UIContext::EnableDebugger(bool enable)
{
    if (_context == nullptr)
    {
        return;
    }

    if (!_debuggerInitialized)
    {
        _debuggerInitialized = Rml::Debugger::Initialise(_context);
    }

    if (_debuggerInitialized)
    {
        Rml::Debugger::SetVisible(enable);
    }
}

Rml::Element *UIContext::GetElementById(const std::string &elementId)
{
    if (_document == nullptr)
    {
        return nullptr;
    }

    return _document->GetElementById(elementId);
}

bool UIContext::RegisterEventListener(Rml::Element &element, const Rml::String &eventType,
                                      std::function<void(Rml::Event &)> callback, bool useCapture)
{
    auto listener = std::make_unique<CallbackEventListener>(std::move(callback));
    element.AddEventListener(eventType, listener.get(), useCapture);
    _eventListeners.push_back(std::move(listener));
    return true;
}

bool UIContext::ProcessKey(int key, int action, int mods)
{
    if (_context == nullptr)
    {
        return false;
    }

    const Rml::Input::KeyIdentifier keyId = ToRmlKey(key);
    const int modifiers = ToRmlModifiers(mods);

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        return _context->ProcessKeyDown(keyId, modifiers);
    }
    if (action == GLFW_RELEASE)
    {
        return _context->ProcessKeyUp(keyId, modifiers);
    }
    return false;
}

bool UIContext::ProcessText(unsigned int codepoint)
{
    if (_context == nullptr)
    {
        return false;
    }
    return _context->ProcessTextInput(static_cast<Rml::Character>(codepoint));
}

bool UIContext::ProcessMouseMove(double x, double y, int mods)
{
    if (_context == nullptr)
    {
        return false;
    }
    return _context->ProcessMouseMove(static_cast<int>(x), static_cast<int>(y), ToRmlModifiers(mods));
}

bool UIContext::ProcessMouseButton(int button, int action, int mods)
{
    if (_context == nullptr)
    {
        return false;
    }

    const int mouseButton = ToRmlMouseButtonIndex(button);
    const int modifiers = ToRmlModifiers(mods);

    if (action == GLFW_PRESS)
    {
        return _context->ProcessMouseButtonDown(mouseButton, modifiers);
    }
    if (action == GLFW_RELEASE)
    {
        return _context->ProcessMouseButtonUp(mouseButton, modifiers);
    }
    return false;
}

bool UIContext::ProcessMouseWheel(double xoffset, double yoffset, int mods)
{
    (void) xoffset; // TODO: Need to check and handle if needed
    if (_context == nullptr)
    {
        return false;
    }
    return _context->ProcessMouseWheel(static_cast<float>(yoffset), ToRmlModifiers(mods));
}
} // namespace Rmlui::Resource
