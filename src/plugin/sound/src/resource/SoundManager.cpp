#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.hpp"
#include <miniaudio.h>

namespace ES::Plugin::Sound::Resource {
SoundManager::~SoundManager()
{
    for (auto &[name, sound] : _soundsToPlay)
    {
        ma_decoder_uninit(&sound.decoder);
    }

    if (_isInitialized)
    {
        ma_device_stop(&_device);
        ma_device_uninit(&_device);
    }
}

} // namespace ES::Plugin::Sound::Resource
