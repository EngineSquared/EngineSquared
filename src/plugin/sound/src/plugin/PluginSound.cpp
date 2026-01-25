#include "plugin/PluginSound.hpp"
#include "resource/SoundManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/InitSound.hpp"

void Sound::Plugin::Bind()
{
    RegisterResource<Resource::SoundManager>(Resource::SoundManager());
    RegisterSystems<Engine::Scheduler::Startup>(Sound::System::InitSounds);
    RegisterSystems<Engine::Scheduler::Update>(Sound::System::ReportAudioErrors);
}
