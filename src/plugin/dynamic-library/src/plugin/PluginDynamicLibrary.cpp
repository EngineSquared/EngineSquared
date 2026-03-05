#include "plugin/PluginDynamicLibrary.hpp"
#include "resource/ComponentsMeta.hpp"
#include "scheduler/Startup.hpp"
#include "system/LoadDynamicPlugins.hpp"

void DynamicLibrary::Plugin::Bind()
{
    RegisterResource(Resource::ComponentsMeta{});
    RegisterSystems<Engine::Scheduler::Startup>(System::LoadDynamicPlugins);
}
