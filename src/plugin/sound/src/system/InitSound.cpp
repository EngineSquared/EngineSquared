#include "system/InitSound.hpp"
#include "resource/SoundManager.hpp"

void Plugin::Sound::System::InitSounds(Engine::Core &core)
{
    core.GetResource<Plugin::Sound::Resource::SoundManager>().Init();
}
