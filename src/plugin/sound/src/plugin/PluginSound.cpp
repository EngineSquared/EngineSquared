#include "PluginSound.hpp"
#include "SoundManager.hpp"
#include "InitSound.hpp"
#include "Update.hpp"

void ES::Plugin::Sound::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Sound::Resource::SoundManager>(ES::Plugin::Sound::Resource::SoundManager());
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Sound::System::InitSounds);
}