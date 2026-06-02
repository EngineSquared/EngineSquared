#include "plugin/PluginDynamicPlugin.hpp"
#include "resource/DynamicLibraryContainer.hpp"
#include "resource/RegistryMediator.hpp"

void DynamicPlugin::Plugin::Attach()
{
    RegisterResource(DynamicPlugin::Resource::RegistryMediator{});
    RegisterResource(DynamicPlugin::Resource::DynamicLibraryContainer{});
}
