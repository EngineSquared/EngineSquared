#include "PluginNativeScripting.hpp"
#include "system/ScriptingSystem.hpp"
#include "Startup.hpp"

void ES::Plugin::NativeScripting::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::NativeScripting::System::SetOnDestroy);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::NativeScripting::System::UpdateScripts);
}
