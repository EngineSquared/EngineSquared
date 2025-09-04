#include "plugin/PluginSound.hpp"
#include "system/InitSound.hpp"
#include "resource/SoundManager.hpp"
#include "scheduler/Startup.hpp"

void ES::Plugin::Sound::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Sound::Resource::SoundManager>(ES::Plugin::Sound::Resource::SoundManager());
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Sound::System::InitSounds);
}
