/**************************************************************************
 * EngineSquared - Sound Plugin Usage Example
 *
 * This example demonstrates how to use the Sound plugin.
 **************************************************************************/

#include "Engine.hpp"
#include "Sound.hpp"

void SetupSound(Engine::Core &core)
{
    auto &soundMgr = core.GetResource<Sound::Resource::SoundManager>();
    soundMgr.RegisterSound("engine_low", "src/plugin/sound/examples/SoundUsage/assets/start-menu.mp3", true);
    soundMgr.Play("engine_low");
}

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Sound::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(SetupSound);

    core.Run();

    return 0;
}
