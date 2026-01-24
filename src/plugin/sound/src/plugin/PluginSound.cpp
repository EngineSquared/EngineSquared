#include "plugin/PluginSound.hpp"
#include "resource/SoundManager.hpp"
#include "scheduler/Startup.hpp"
#include "system/InitSound.hpp"

void Sound::Plugin::Bind()
{
    // SoundManager is created in-place via system instead of RegisterResource
    // because it contains non-movable members (std::mutex, ma_device)
    RegisterSystems<Engine::Scheduler::Startup>(Sound::System::InitSounds);
}
