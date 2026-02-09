#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.hpp"
#include <miniaudio.h>

namespace Sound::Resource {
SoundManager::~SoundManager()
{
    if (_deviceInit)
    {
        ma_device_stop(&_device);
    }

    for (auto &[name, sound] : _soundsToPlay)
    {
        if (sound.hasEngineSound)
            ma_sound_uninit(&sound.engineSound);
        if (sound.decoderInitialized)
            ma_decoder_uninit(&sound.decoder);
    }

    if (_engineInit)
    {
        ma_engine_uninit(&_engine);
    }

    if (_deviceInit)
    {
        ma_device_uninit(&_device);
    }
}

} // namespace Sound::Resource
