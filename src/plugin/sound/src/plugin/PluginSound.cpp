#include "plugin/PluginSound.hpp"
#include "resource/SoundManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/InitSound.hpp"

void Plugin::Sound::Plugin::Bind()
{
    RegisterResource<Resource::SoundManager>(Resource::SoundManager());
    RegisterSystems<Engine::Scheduler::Startup>(Sound::System::InitSounds);
}
