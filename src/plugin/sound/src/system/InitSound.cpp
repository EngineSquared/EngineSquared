#include "system/InitSound.hpp"
#include "resource/SoundManager.hpp"

void Sound::System::InitSounds(Engine::Core &core)
{
    // Create SoundManager in-place (required for non-movable types with std::mutex)
    core.GetRegistry().ctx().emplace<Sound::Resource::SoundManager>();

    // Initialize the sound manager
    core.GetResource<Sound::Resource::SoundManager>().Init(core);
}
