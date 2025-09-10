#include "system/InitSound.hpp"
#include "resource/SoundManager.hpp"

void ES::Plugin::Sound::System::InitSounds(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Init(core);
}
