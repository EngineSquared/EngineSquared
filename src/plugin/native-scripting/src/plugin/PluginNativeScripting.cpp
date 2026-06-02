#include "plugin/PluginNativeScripting.hpp"
#include "scheduler/Update.hpp"
#include "system/ScriptingSystem.hpp"

void NativeScripting::Plugin::Attach()
{
    RegisterSystems<Engine::Scheduler::Startup>(System::SetOnDestroy);
    RegisterSystems<Engine::Scheduler::Update>(System::UpdateScripts);
}
