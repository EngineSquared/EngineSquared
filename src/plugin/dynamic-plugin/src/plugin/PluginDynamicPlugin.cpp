#include "plugin/PluginDynamicPlugin.hpp"
#include "resource/LibraryMediator.hpp"

void DynamicPlugin::Plugin::Bind() { RegisterResource(DynamicPlugin::Resource::LibraryMediator{}); }
