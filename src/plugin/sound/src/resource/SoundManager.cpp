#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.hpp"

namespace ES::Plugin::Sound::Resource {
SoundManager::~SoundManager()
{
    for (auto &[name, sound] : _soundsToPlay)
    {
        ma_decoder_uninit(&sound.decoder);
    }
    ma_device_uninit(&_device);
}

} // namespace ES::Plugin::Sound::Resource
