#include "plugin/PluginNativeScripting.hpp"
#include "scheduler/Update.hpp"
#include "system/ScriptingSystem.hpp"

void Plugin::NativeScripting::Plugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Startup>(System::SetOnDestroy);
    RegisterSystems<Engine::Scheduler::Update>(System::UpdateScripts);
}
