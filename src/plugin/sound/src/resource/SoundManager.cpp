#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.hpp"
#include <miniaudio.h>

namespace Sound::Resource {
SoundManager::~SoundManager()
{
    for (auto &[name, sound] : _soundsToPlay)
    {
        if (sound.hasEngineSound)
            ma_sound_uninit(&sound.engineSound);
        ma_decoder_uninit(&sound.decoder);
    }

    if (_engineInit)
    {
        ma_engine_uninit(&_engine);
    }

    if (_deviceInit)
    {
        ma_device_stop(&_device);
        ma_device_uninit(&_device);
    }
}

} // namespace Sound::Resource
