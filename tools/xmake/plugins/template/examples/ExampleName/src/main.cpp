#include "Plugin.hpp"

int main(void)
{
    Engine::Core core;

    core.AddPlugins<PluginNamespace::Plugin>();

    core.RegisterSystem([](Engine::Core &core) { Log::Info("Hello World"); });

    core.RunSystems();

    return 0;
}
