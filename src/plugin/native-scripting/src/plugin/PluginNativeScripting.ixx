module;

#include "APlugin.hpp"
#include "Startup.hpp"
#include "Update.hpp"

export module ESPluginNativeScriptingPlugin;

import ESPluginNativeScriptingSystem;

export namespace ES::Plugin::NativeScripting {
    class Plugin : public ES::Engine::APlugin {
    public:
        explicit Plugin(ES::Engine::Core &core) : APlugin(core) {}

        void Bind() final {
            RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::NativeScripting::System::SetOnDestroy);
            RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::NativeScripting::System::UpdateScripts);
        }
    };
}