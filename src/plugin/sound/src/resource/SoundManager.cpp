#define MINIAUDIO_IMPLEMENTATION
#include "SoundManager.hpp"
#include <miniaudio.h>

namespace Sound::Resource {
SoundManager::~SoundManager()
{
    // Teardown order: engine first (if it owns/manages device context), then device
    {
        std::scoped_lock lock(soundsMutex);  // Protect map iteration during destruction
        for (auto &[name, sound] : _soundsToPlay)
        {
            {
                std::scoped_lock lock(sound.engineMutex);
                if (sound.hasEngineSound.load(std::memory_order_acquire))
                {
                    ma_sound_uninit(&sound.engineSound);
                    sound.hasEngineSound.store(false, std::memory_order_release);
                }
            }
            ma_decoder_uninit(&sound.decoder);
        }
    }

    // Uninitialize engine before device to avoid device context issues
    if (_engineInit)
    {
        ma_engine_uninit(&_engine);
    }

    // Only stop and uninit device if it was successfully initialized
    if (_deviceInit)
    {
        ma_device_stop(&_device);
        ma_device_uninit(&_device);
    }
}

} // namespace Sound::Resource
