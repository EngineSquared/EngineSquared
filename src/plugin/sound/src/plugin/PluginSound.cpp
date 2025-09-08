#include "plugin/PluginSound.hpp"
#include "resource/SoundManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/InitSound.hpp"

void ES::Plugin::Sound::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Sound::Resource::SoundManager>(ES::Plugin::Sound::Resource::SoundManager());
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Sound::System::InitSounds);
}
