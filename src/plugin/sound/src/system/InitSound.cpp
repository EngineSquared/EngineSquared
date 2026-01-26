#include "system/InitSound.hpp"
#include "resource/SoundManager.hpp"

void Sound::System::InitSounds(Engine::Core &core) { core.GetResource<Sound::Resource::SoundManager>().Init(core); }

void Sound::System::ReportAudioErrors(Engine::Core &core)
{
    core.GetResource<Sound::Resource::SoundManager>().CheckCallbackErrors();
}
