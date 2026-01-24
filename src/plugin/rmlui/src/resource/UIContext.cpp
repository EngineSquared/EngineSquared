#include "resource/UIContext.hpp"
#include "RmlUi/Core/ElementDocument.h"
#include "resource/InputManager.hpp"
#include "resource/Window.hpp"

#include <GLFW/glfw3.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "Logger.hpp"
#include "core/Core.hpp"
#include "fmt/format.h"
#include "spdlog/fmt/bundled/format.h"

#include "RmlUi/Config/Config.h"
#include "RmlUi/Core/Core.h"
#include "RmlUi/Core/Event.h"
#include "RmlUi/Core/EventListener.h"
#include "RmlUi/Core/Input.h"
#include "RmlUi/Core/Math.h"
#include "RmlUi/Core/Types.h"
#include "RmlUi/Debugger/Debugger.h"

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

    static const std::unordered_map<int, KI> kKeyMap = {
        {GLFW_KEY_SPACE,         KI::KI_SPACE      },
        {GLFW_KEY_APOSTROPHE,    KI::KI_OEM_7      },
        {GLFW_KEY_COMMA,         KI::KI_OEM_COMMA  },
        {GLFW_KEY_MINUS,         KI::KI_OEM_MINUS  },
        {GLFW_KEY_PERIOD,        KI::KI_OEM_PERIOD },
        {GLFW_KEY_SLASH,         KI::KI_OEM_2      },
        {GLFW_KEY_SEMICOLON,     KI::KI_OEM_1      },
        {GLFW_KEY_EQUAL,         KI::KI_OEM_PLUS   },
        {GLFW_KEY_LEFT_BRACKET,  KI::KI_OEM_4      },
        {GLFW_KEY_BACKSLASH,     KI::KI_OEM_5      },
        {GLFW_KEY_RIGHT_BRACKET, KI::KI_OEM_6      },
        {GLFW_KEY_GRAVE_ACCENT,  KI::KI_OEM_3      },
        {GLFW_KEY_ENTER,         KI::KI_RETURN     },
        {GLFW_KEY_ESCAPE,        KI::KI_ESCAPE     },
        {GLFW_KEY_BACKSPACE,     KI::KI_BACK       },
        {GLFW_KEY_TAB,           KI::KI_TAB        },
        {GLFW_KEY_INSERT,        KI::KI_INSERT     },
        {GLFW_KEY_DELETE,        KI::KI_DELETE     },
        {GLFW_KEY_RIGHT,         KI::KI_RIGHT      },
        {GLFW_KEY_LEFT,          KI::KI_LEFT       },
        {GLFW_KEY_DOWN,          KI::KI_DOWN       },
        {GLFW_KEY_UP,            KI::KI_UP         },
        {GLFW_KEY_PAGE_UP,       KI::KI_PRIOR      },
        {GLFW_KEY_PAGE_DOWN,     KI::KI_NEXT       },
        {GLFW_KEY_HOME,          KI::KI_HOME       },
        {GLFW_KEY_END,           KI::KI_END        },
        {GLFW_KEY_CAPS_LOCK,     KI::KI_CAPITAL    },
        {GLFW_KEY_SCROLL_LOCK,   KI::KI_SCROLL     },
        {GLFW_KEY_NUM_LOCK,      KI::KI_NUMLOCK    },
        {GLFW_KEY_PRINT_SCREEN,  KI::KI_SNAPSHOT   },
        {GLFW_KEY_PAUSE,         KI::KI_PAUSE      },
        {GLFW_KEY_KP_DECIMAL,    KI::KI_DECIMAL    },
        {GLFW_KEY_KP_DIVIDE,     KI::KI_DIVIDE     },
        {GLFW_KEY_KP_MULTIPLY,   KI::KI_MULTIPLY   },
        {GLFW_KEY_KP_SUBTRACT,   KI::KI_SUBTRACT   },
        {GLFW_KEY_KP_ADD,        KI::KI_ADD        },
        {GLFW_KEY_KP_ENTER,      KI::KI_NUMPADENTER},
        {GLFW_KEY_LEFT_SHIFT,    KI::KI_LSHIFT     },
        {GLFW_KEY_RIGHT_SHIFT,   KI::KI_RSHIFT     },
        {GLFW_KEY_LEFT_CONTROL,  KI::KI_LCONTROL   },
        {GLFW_KEY_RIGHT_CONTROL, KI::KI_RCONTROL   },
        {GLFW_KEY_LEFT_ALT,      KI::KI_LMENU      },
        {GLFW_KEY_RIGHT_ALT,     KI::KI_RMENU      },
        {GLFW_KEY_LEFT_SUPER,    KI::KI_LWIN       },
        {GLFW_KEY_RIGHT_SUPER,   KI::KI_RWIN       },
    };

    if (const auto it = kKeyMap.find(key); it != kKeyMap.end())
    {
        return it->second;
    }
    return KI::KI_UNKNOWN;
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
    if (_inputCallbacksRegistered && core.HasResource<Input::Resource::InputManager>())
    {
        auto &input = core.GetResource<Input::Resource::InputManager>();
        if (_inputCallbackIds.keyCallbackId)
        {
            input.DeleteKeyCallback(*_inputCallbackIds.keyCallbackId);
        }
        if (_inputCallbackIds.charCallbackId)
        {
            input.DeleteCharCallback(*_inputCallbackIds.charCallbackId);
        }
        if (_inputCallbackIds.mouseButtonCallbackId)
        {
            input.DeleteMouseButtonCallback(*_inputCallbackIds.mouseButtonCallbackId);
        }
        if (_inputCallbackIds.cursorPosCallbackId)
        {
            input.DeleteCursorPosCallback(*_inputCallbackIds.cursorPosCallbackId);
        }
        if (_inputCallbackIds.scrollCallbackId)
        {
            input.DeleteScrollCallback(*_inputCallbackIds.scrollCallbackId);
        }
    }
    _inputCallbackIds = {};
    _inputCallbacksRegistered = false;

    if (_document != nullptr)
    {
        _document->Close();
        _document = nullptr;
    }
    for (auto const &[path, document] : _overlayDocuments)
    {
        (void) path;
        if (document != nullptr)
        {
            document->Close();
        }
    }
    _overlayDocuments.clear();
    if (_context != nullptr)
    {
        Rml::RemoveContext(_context->GetName());
        _context = nullptr;
    }
    _titleCache.clear();
    Rml::Shutdown();
    _eventListeners.clear();
    _inputCallbacksRegistered = false;
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

    for (auto &entry : _eventListeners)
    {
        if (entry.element != nullptr)
        {
            entry.element->RemoveEventListener(entry.eventType, entry.listener.get(), entry.useCapture);
        }
    }
    _eventListeners.clear();

    if (_document != nullptr)
    {
        _document->Close();
        _document = nullptr;
    }
    for (auto const &[path, document] : _overlayDocuments)
    {
        (void) path;
        if (document != nullptr)
        {
            document->Close();
        }
    }
    _overlayDocuments.clear();
    if (!_debuggerInitialized)
    {
        _context->UnloadAllDocuments();
    }
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

bool UIContext::LoadOverlayDocument(const std::string &docPath)
{
    if (_context == nullptr)
    {
        throw Rmlui::Exception::ReadRmlDocumentError(
            fmt::format("Rmlui can not load overlay document due to not being initialized: {}", docPath));
    }

    if (_overlayDocuments.contains(docPath))
    {
        return true;
    }

    auto *document = _context->LoadDocument(docPath);
    if (document == nullptr)
    {
        throw Rmlui::Exception::ReadRmlDocumentError(fmt::format("Rmlui can not load overlay document: {}", docPath));
    }

    document->Show();
    _overlayDocuments.emplace(docPath, document);
    return true;
}

bool UIContext::UnloadOverlayDocument(const std::string &docPath)
{
    auto it = _overlayDocuments.find(docPath);

    if (it == _overlayDocuments.end())
    {
        throw Rmlui::Exception::ReadRmlDocumentError(
            fmt::format("Rmlui can not unload overlay document due to not being loaded: {}", docPath));
    }
    if (it->second != nullptr)
    {
        it->second->Close();
    }
    _overlayDocuments.erase(it);
    return true;
}

const std::string &UIContext::GetTitle() const { return _titleCache; }

Rml::ElementDocument *UIContext::GetDocument() { return _document; }

const Rml::ElementDocument *UIContext::GetDocument() const { return _document; }

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

    if (auto *element = _document->GetElementById(elementId))
    {
        return element;
    }

    for (auto &[path, document] : _overlayDocuments)
    {
        (void) path;
        if (document == nullptr)
        {
            continue;
        }
        if (auto *element = document->GetElementById(elementId))
        {
            return element;
        }
    }

    return nullptr;
}

bool UIContext::RegisterEventListener(Rml::Element &element, const Rml::String &eventType,
                                      std::function<void(Rml::Event &)> callback, bool useCapture)
{
    auto listener = std::make_unique<CallbackEventListener>(std::move(callback));
    element.AddEventListener(eventType, listener.get(), useCapture);
    _eventListeners.push_back(UIContext::EventListenerEntry{
        .element = &element,
        .eventType = eventType,
        .useCapture = useCapture,
        .listener = std::move(listener),
    });
    return true;
}

bool UIContext::UnregisterEventListener(Rml::Element &element, const Rml::String &eventType)
{
    for (auto it = _eventListeners.begin(); it != _eventListeners.end(); ++it)
    {
        if (it->element != &element || it->eventType != eventType)
        {
            continue;
        }
        element.RemoveEventListener(eventType, it->listener.get());
        _eventListeners.erase(it);
        return true;
    }
    return false;
}

void UIContext::SetInputCallbackIds(const InputCallbackIds &ids) { _inputCallbackIds = ids; }

bool UIContext::AreInputCallbacksRegistered() const { return _inputCallbacksRegistered; }

void UIContext::SetInputCallbacksRegistered(bool registered) { _inputCallbacksRegistered = registered; }

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
    if (_context == nullptr)
    {
        return false;
    }

    const int modifiers = ToRmlModifiers(mods);
    bool handled = false;
    if (xoffset != 0.0)
    {
        handled |= _context->ProcessMouseWheel(static_cast<float>(xoffset), modifiers | Rml::Input::KM_SHIFT);
    }
    if (yoffset != 0.0)
    {
        handled |= _context->ProcessMouseWheel(static_cast<float>(yoffset), modifiers);
    }
    return handled;
}
} // namespace Rmlui::Resource
