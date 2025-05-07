#include "InitSound.hpp"
#include "SoundManager.hpp"

void ES::Plugin::Sound::System::InitSounds(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::Sound::Resource::SoundManager>().Init();
}
