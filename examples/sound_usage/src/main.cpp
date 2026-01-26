/**************************************************************************
 * EngineSquared - Sound Plugin Usage Example
 *
 * This example demonstrates how to use the Sound plugin.
 **************************************************************************/

#include "Engine.hpp"
#include "Sound.hpp"

#include "export.h"

constexpr std::string_view FILES_PATH =
#ifdef PATH_ASSETS
    PATH_ASSETS;
#else
    "./assets/";
#endif

void SetupSound(Engine::Core &core)
{
    auto &soundMgr = core.GetResource<Sound::Resource::SoundManager>();
    soundMgr.RegisterSound("engine_low", std::string(FILES_PATH) + "start-menu.mp3", true);
    soundMgr.Play("engine_low");
}

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Sound::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>(SetupSound);

    core.RunCore();

    return 0;
}
