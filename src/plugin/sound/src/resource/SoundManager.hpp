#pragma once

#include <functional>
#include <map>
#include <miniaudio.h>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <cstdint>
#include <array>
#include <cstring>
#include <atomic>
#include <mutex>

#include "Engine.hpp"
#include "FunctionContainer.hpp"

namespace Sound::Resource {

using CustomDataCallback = std::function<void(ma_device *pDevice, void *pOutput, ma_uint32 frameCount)>;

class SoundManager {
  private:
    ma_result _result;
    ma_device_config _deviceConfig;
    ma_device _device;
    bool _deviceInit = false;  // Guard for device cleanup
    ma_engine _engine;
    bool _engineInit = false;

    struct Sound {
        std::string name;
        std::string path;
        ma_decoder decoder;
        bool loop = false;
        std::atomic<bool> isPlaying{false};  // Lock-free state
        std::atomic<bool> isPaused{false};   // Lock-free state
        std::atomic<float> volume{1.0f};
        ma_uint64 loopStartFrame = 0;
        ma_uint64 loopEndFrame = 0;
        // High-level engine sound for pitch shifting (lazy init)
        ma_sound engineSound{};
        std::atomic<bool> hasEngineSound{false};
        std::atomic<bool> usingEngine{false}; // when true, playback handled by ma_engine
        std::mutex engineMutex;               // Protects engineSound + flags during moves/ops

        // Sound is not copyable (atomics), but must be movable for unordered_map
        Sound() = default;
        ~Sound() = default;
        Sound(const Sound&) = delete;
        Sound& operator=(const Sound&) = delete;
        Sound(Sound&& other) noexcept
        {
            std::scoped_lock lock(other.engineMutex);
            name = std::move(other.name);
            path = std::move(other.path);
            decoder = other.decoder;
            loop = other.loop;
            isPlaying.store(other.isPlaying.load(std::memory_order_acquire), std::memory_order_release);
            isPaused.store(other.isPaused.load(std::memory_order_acquire), std::memory_order_release);
            volume.store(other.volume.load(std::memory_order_acquire), std::memory_order_release);
            loopStartFrame = other.loopStartFrame;
            loopEndFrame = other.loopEndFrame;
            engineSound = other.engineSound;
            hasEngineSound.store(other.hasEngineSound.exchange(false, std::memory_order_acq_rel),
                                 std::memory_order_release);
            usingEngine.store(other.usingEngine.exchange(false, std::memory_order_acq_rel),
                              std::memory_order_release);

            other.decoder = ma_decoder{};
            other.engineSound = ma_sound{};
            other.isPlaying.store(false, std::memory_order_release);
            other.isPaused.store(false, std::memory_order_release);
            other.volume.store(1.0f, std::memory_order_release);
        }
        Sound& operator=(Sound&& other) noexcept {
            if (this != &other) {
                std::scoped_lock lock(engineMutex, other.engineMutex);
                name = std::move(other.name);
                path = std::move(other.path);
                decoder = other.decoder;
                loop = other.loop;
                isPlaying.store(other.isPlaying.load(std::memory_order_acquire), std::memory_order_release);
                isPaused.store(other.isPaused.load(std::memory_order_acquire), std::memory_order_release);
                volume.store(other.volume.load(std::memory_order_acquire), std::memory_order_release);
                loopStartFrame = other.loopStartFrame;
                loopEndFrame = other.loopEndFrame;
                engineSound = other.engineSound;
                hasEngineSound.store(other.hasEngineSound.exchange(false, std::memory_order_acq_rel),
                                     std::memory_order_release);
                usingEngine.store(other.usingEngine.exchange(false, std::memory_order_acq_rel),
                                  std::memory_order_release);

                other.decoder = ma_decoder{};
                other.engineSound = ma_sound{};
                other.isPlaying.store(false, std::memory_order_release);
                other.isPaused.store(false, std::memory_order_release);
                other.volume.store(1.0f, std::memory_order_release);
            }
            return *this;
        }
    };

    struct TransparentHash {
        using is_transparent = void;

        std::size_t operator()(std::string_view key) const noexcept { return std::hash<std::string_view>{}(key); }
    };

    struct TransparentEqual {
        using is_transparent = void;

        bool operator()(std::string_view lhs, std::string_view rhs) const noexcept { return lhs == rhs; }
    };

    std::unordered_map<std::string, Sound, TransparentHash, TransparentEqual> _soundsToPlay;
    std::mutex soundsMutex;  // Protects _soundsToPlay map from concurrent modification

    FunctionUtils::FunctionContainer<void, ma_device *, void *, ma_uint32> _customCallbacks;

  public:
    /**
     * Constructor.
     */
    SoundManager() = default;

    /**
     * Destructor.
     */
    ~SoundManager();

    // Copy and move operations implicitly deleted due to non-copyable/non-movable members
    // (std::mutex, ma_device, etc.)

    /**
     * @brief Audio data callback used by the playback device to fill the output buffer.
     *
     * @param pDevice Pointer to the audio playback device.
     * @param pOutput Pointer to the output buffer that will be filled with audio samples.
     * @param pInput  Unused input buffer (typically nullptr for playback-only).
     * @param frameCount Number of audio frames that must be written to the output buffer.
     *
     * @return void
     */
    static void data_callback(ma_device *pDevice, void *pOutput, const void * /*pInput*/, ma_uint32 frameCount)
    {
        auto *core = static_cast<Engine::Core *>(pDevice->pUserData);
        auto &self = core->GetResource<SoundManager>();

        // Clear output in device's native format
        // This is crucial - we need to clear in the device format, not always f32
        std::memset(pOutput, 0, ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels) * frameCount);

        const auto &callbacks = self._customCallbacks.GetFunctions();
        for (const auto &callback : callbacks)
        {
            (*callback)(pDevice, pOutput, frameCount);
        }

        // For now, only support one sound at a time or implement proper format conversion
        // This is simpler and more robust than trying to mix different formats
        {
            std::scoped_lock lock(self.soundsMutex);  // Protect map iteration from RegisterSound/UnregisterSound
            for (auto &[name, sound] : self._soundsToPlay)
            {
                // Skip sounds that are using the high-level engine (played elsewhere)
                if (sound.usingEngine.load(std::memory_order_acquire))
                    continue;

            if (!sound.isPlaying.load(std::memory_order_acquire) || sound.isPaused.load(std::memory_order_acquire))
                continue;

            // Read directly from decoder into the device output buffer
            ma_uint64 framesRead = 0;
            ma_result result = ma_decoder_read_pcm_frames(&sound.decoder, pOutput, frameCount, &framesRead);

            if (result != MA_SUCCESS && result != MA_AT_END)
            {
                Log::Error(fmt::format("[Audio] Decoder error: {}", ma_result_description(result)));
            }

            const float currentVolume = sound.volume.load(std::memory_order_acquire);
            // Apply volume to decoded samples (s16 format for 911 RSR WAV)
            if (currentVolume < 0.99f || currentVolume > 1.01f)
            {
                int16_t *pInt16 = static_cast<int16_t *>(pOutput);
                ma_uint32 sampleCount = static_cast<ma_uint32>(framesRead * pDevice->playback.channels);
                for (ma_uint32 i = 0; i < sampleCount; ++i)
                {
                    float sample = static_cast<float>(pInt16[i]) * currentVolume;
                    if (sample > 32767.0f) sample = 32767.0f;
                    if (sample < -32768.0f) sample = -32768.0f;
                    pInt16[i] = static_cast<int16_t>(sample);
                }
            }

            // If we didn't read enough frames and looping is enabled
            if (framesRead < frameCount)
            {
                if (sound.loop)
                {
                    // Seek to loop start and read remaining frames
                    ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                    void* offsetOutput = (char*)pOutput + (ma_get_bytes_per_frame(pDevice->playback.format, pDevice->playback.channels) * framesRead);
                    ma_uint64 remainingFrames = frameCount - framesRead;
                    ma_uint64 additionalFramesRead = 0;

                    result = ma_decoder_read_pcm_frames(&sound.decoder, offsetOutput, remainingFrames, &additionalFramesRead);
                    framesRead += additionalFramesRead;
                }
                else
                {
                    // Not looping, stop playback at end
                    if (framesRead < frameCount && result == MA_AT_END)
                    {
                        sound.isPlaying.store(false, std::memory_order_release);
                    }
                }
            }

            // Check loop end point
            if (sound.loop && sound.loopEndFrame > 0)
            {
                ma_uint64 currentFrame;
                if (ma_decoder_get_cursor_in_pcm_frames(&sound.decoder, &currentFrame) == MA_SUCCESS)
                {
                    if (currentFrame >= sound.loopEndFrame)
                    {
                        ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                    }
                }
            }

            // Only play one sound at a time for now (simplicity)
            break;
            }  // End lock scope
        }
    }

    /**
     * @brief Initialize the sound system.
     *
     * Note: We use ma_format_unknown to let miniaudio negotiate the best format.
     * Most decoders output ma_format_s16 (16-bit PCM), and miniaudio will handle
     * conversion if needed. This avoids format mismatch issues.
     *
     * @return void
     */
    inline void Init(Engine::Core &core)
    {
        _deviceConfig = ma_device_config_init(ma_device_type_playback);
        _deviceConfig.playback.format = ma_format_unknown;  // Let miniaudio choose best format
        _deviceConfig.playback.channels = 2;                // Stereo
        _deviceConfig.sampleRate = 44100;                   // Standard sample rate
        _deviceConfig.dataCallback = SoundManager::data_callback;
        _deviceConfig.pUserData = &core;

        Log::Info(fmt::format("[Audio] Initializing device: format=auto-negotiate, channels=2, sampleRate=44100"));

        _result = ma_device_init(NULL, &_deviceConfig, &_device);
        if (_result != MA_SUCCESS)
        {
            Log::Error(fmt::format("Failed to init audio device: {}", ma_result_description(_result)));
            return;
        }

        _result = ma_device_start(&_device);
        if (_result != MA_SUCCESS)
        {
            Log::Error(fmt::format("Failed to start audio device: {}", ma_result_description(_result)));
            ma_device_uninit(&_device);
        }
        else
        {
            _deviceInit = true;  // Mark device as successfully initialized
            Log::Info(fmt::format("[Audio] Device started successfully. Device format={}, sample rate={}, channels={}",
                                 static_cast<int>(_device.playback.format), _device.sampleRate, _device.playback.channels));
        }
    }

    /**
     * @brief Register the provided sound file.
     *
     * @param soundName Name to assign to the registered sound
     * @param filePath Path of the sound file to register
     * @param loop Set the sound param to loop, default: False
     *
     * @return void
     */
    inline void RegisterSound(const std::string &soundName, const std::string &filePath, bool loop = false)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map access from data_callback
        if (_soundsToPlay.contains(soundName))
        {
            Log::Warn(fmt::format("Could not register: Sound \"{}\" already exists", soundName));
            return;
        }

        Sound sound;
        sound.name = soundName;
        sound.path = filePath;
        sound.loop = loop;

        _result = ma_decoder_init_file(filePath.c_str(), nullptr, &sound.decoder);
        if (_result != MA_SUCCESS)
        {
            Log::Error(fmt::format("Failed to initialize the audio device: {}", ma_result_description(_result)));
            return;
        }
        _soundsToPlay.emplace(soundName, std::move(sound));
    }

    /**
     * @brief Unregister the target sound file from the sound vector.
     *
     * @param soundName Name of the target sound to remove
     *
     * @return void
     */
    inline void UnregisterSound(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map access from data_callback
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            ma_decoder_uninit(&it->second.decoder);
            _soundsToPlay.erase(it);
        }
        else
        {
            Log::Error(fmt::format("Could not unregister: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Play the target sound
     *
     * @param soundName Name of the target sound to play
     *
     * @return void
     */
    inline void Play(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            snd.isPlaying.store(true, std::memory_order_release);
            snd.isPaused.store(false, std::memory_order_release);
            if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
            {
                std::scoped_lock lock(snd.engineMutex);
                if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
                {
                    ma_sound_start(&snd.engineSound);
                }
            }
        }
        else
        {
            Log::Error(fmt::format("Could not play: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Stop the target sound
     *
     * @param soundName Name of the target sound to stop
     *
     * @return void
     */
    inline void Stop(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            snd.isPlaying.store(false, std::memory_order_release);
            snd.isPaused.store(false, std::memory_order_release);
            if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
            {
                std::scoped_lock lock(snd.engineMutex);
                if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
                {
                    ma_sound_stop(&snd.engineSound);
                    ma_sound_seek_to_pcm_frame(&snd.engineSound, 0);
                }
            }
            else
            {
                ma_decoder_seek_to_pcm_frame(&snd.decoder, 0);
            }
        }
        else
        {
            Log::Error(fmt::format("Could not stop: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Pause the target sound
     *
     * @param soundName Name of the target sound to pause
     *
     * @return bool
     */
    inline void Pause(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            snd.isPaused.store(true, std::memory_order_release);
            if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
            {
                std::scoped_lock lock(snd.engineMutex);
                if (snd.usingEngine.load(std::memory_order_acquire) && snd.hasEngineSound.load(std::memory_order_acquire))
                {
                    ma_sound_stop(&snd.engineSound);
                }
            }
        }
        else
        {
            Log::Error(fmt::format("Could not pause: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Check if the target sound is playing
     *
     * @param soundName Name of the target sound
     *
     * @return bool
     */
    inline bool IsPlaying(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            return it->second.isPlaying.load(std::memory_order_acquire) && !it->second.isPaused.load(std::memory_order_acquire);
        }
        Log::Error(fmt::format("Could not verify playing status: Sound \"{}\" does not exist", soundName));
        return false;
    }

    /**
     * @brief Set the volume of a specific sound.
     *
     * @param soundName Name of the sound.
     * @param volume Volume level (0.0 = silent, 1.0 = full volume).
     */
    inline void SetVolume(const std::string &soundName, float volume)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            it->second.volume.store(std::clamp(volume, 0.0f, 1.0f), std::memory_order_release);
        }
        else
        {
            Log::Error(fmt::format("Could not set volume: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Set the playback pitch (speed) for a sound.
     * @param soundName Name of the sound.
     * @param pitch Pitch factor (> 0), 1.0 = normal.
     * @note Currently disabled - resampler removed for audio quality testing
     */
    inline void SetPitch(const std::string &soundName, float pitch)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            if (pitch <= 0.01f) pitch = 0.01f;

            // Initialize engine and sound lazily
            if (!_engineInit)
            {
                ma_result r = ma_engine_init(NULL, &_engine);
                if (r != MA_SUCCESS)
                {
                    Log::Error(fmt::format("Failed to init ma_engine: {}", ma_result_description(r)));
                    return;
                }
                _engineInit = true;
            }

            std::scoped_lock lock(snd.engineMutex);

            if (!snd.hasEngineSound.load(std::memory_order_acquire))
            {
                ma_result r = ma_sound_init_from_file(&_engine, snd.path.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &snd.engineSound);
                if (r != MA_SUCCESS)
                {
                    Log::Error(fmt::format("Failed to init ma_sound for '{}': {}", snd.name, ma_result_description(r)));
                    return;
                }
                snd.hasEngineSound.store(true, std::memory_order_release);
                snd.usingEngine.store(true, std::memory_order_release);
                ma_sound_set_looping(&snd.engineSound, snd.loop ? MA_TRUE : MA_FALSE);
                ma_sound_set_volume(&snd.engineSound, snd.volume.load(std::memory_order_acquire));

                // If currently marked as playing, start it
                if (snd.isPlaying.load(std::memory_order_acquire) && !snd.isPaused.load(std::memory_order_acquire))
                {
                    ma_sound_start(&snd.engineSound);
                }
            }

            // Apply pitch
            if (snd.hasEngineSound.load(std::memory_order_acquire))
            {
                ma_sound_set_pitch(&snd.engineSound, pitch);
            }
        }
        else
        {
            Log::Error(fmt::format("Could not set pitch: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Set whether the sound should loop.
     *
     * @param soundName Name of the sound.
     * @param shouldLoop Whether the sound should loop.
     */
    inline void SetLoop(const std::string &soundName, bool shouldLoop)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            it->second.loop = shouldLoop;
        }
        else
        {
            Log::Error(fmt::format("Could not set loop: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Set the loop start and end points for a sound.
     *
     * @param soundName Name of the sound.
     * @param startSeconds Time in seconds where the loop starts.
     * @param endSeconds Time in seconds where the loop ends (0 = end of file).
     */
    inline void SetLoopPoints(const std::string &soundName, float startSeconds, float endSeconds = 0)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &decoder = it->second.decoder;
            ma_uint64 sampleRate = decoder.outputSampleRate;
            ma_uint64 totalFrames;
            if (ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames) != MA_SUCCESS)
            {
                Log::Error(
                    fmt::format("Something went wrong while computing PCM frames length of sound \"{}\"", soundName));
                return;
            }

            ma_uint64 startFrame = static_cast<ma_uint64>(startSeconds * sampleRate);
            ma_uint64 endFrame = (endSeconds > 0.0f) ? static_cast<ma_uint64>(endSeconds * sampleRate) : totalFrames;

            if (startFrame >= totalFrames || endFrame > totalFrames || startFrame >= endFrame)
            {
                Log::Warn(fmt::format("Invalid loop range for \"{}\": {}s to {}s, ignored", soundName, startSeconds,
                                      endSeconds));
                return;
            }
            it->second.loopStartFrame = startFrame;
            it->second.loopEndFrame = endFrame;
        }
        else
        {
            Log::Error(fmt::format("Could not set loop points: Sound \"{}\" does not exist", soundName));
        }
    }

    /**
     * @brief Get the current playback position of a sound in seconds.
     *
     * Return value is related to the absolute begin of the sound, not the start frame.
     *
     * @param soundName The name of the target sound to query.
     *
     * @return double The playback position in seconds, or -1.0 on error.
     */
    inline double GetPlayPosition(const std::string &soundName)
    {
        std::scoped_lock lock(soundsMutex);  // Protect map lookup from RegisterSound/UnregisterSound
        auto it = _soundsToPlay.find(soundName);
        if (it == _soundsToPlay.end())
        {
            Log::Error(fmt::format("Could not get the playback position: Sound \"{}\" does not exist", soundName));
            return -1.0f;
        }

        Sound &sound = it->second;
        if (!sound.isPlaying.load(std::memory_order_acquire))
        {
            Log::Warn(fmt::format("Sound \"{}\" is not currently playing", soundName));
            return 0.0f;
        }

        ma_uint64 currentFrame;
        _result = ma_decoder_get_cursor_in_pcm_frames(&sound.decoder, &currentFrame);
        if (_result != MA_SUCCESS)
        {
            Log::Error(fmt::format("Could not get the playback position: {}", ma_result_description(_result)));
            return -1.0f;
        }
        double position = static_cast<double>(currentFrame) / sound.decoder.outputSampleRate;
        return position;
    }

    /**
     * @brief Add a custom audio callback.
     *
     * @param callback The callback function to add.
     * @return FunctionUtils::FunctionID Unique ID of the added callback.
     *
     * @example
     * auto callbackID = soundManager.AddCustomCallback([](ma_device* device, void* output, ma_uint32 frameCount) {});
     */
    inline FunctionUtils::FunctionID AddCustomCallback(const CustomDataCallback &callback)
    {
        return _customCallbacks.AddFunction(callback);
    }

    /**
     * @brief Remove a custom audio callback by its ID.
     *
     * @param id ID of the callback to remove.
     * @return bool True if the callback was removed, false if it didn't exist.
     */
    inline bool RemoveCustomCallback(FunctionUtils::FunctionID id)
    {
        if (!_customCallbacks.Contains(id))
        {
            Log::Error(fmt::format("Could not remove: Custom callback with ID {} does not exist", id));
            return false;
        }
        _customCallbacks.DeleteFunction(id);
        return true;
    }

    /**
     * @brief Check if a custom callback exists by its ID.
     *
     * @param id ID of the callback to check.
     * @return bool True if the callback exists, false otherwise.
     */
    inline bool HasCustomCallback(FunctionUtils::FunctionID id) const { return _customCallbacks.Contains(id); }

    /**
     * @brief Clear all custom callbacks.
     */
    inline void ClearCustomCallbacks()
    {
        // Delete all callbacks by iterating and collecting IDs first
        std::vector<FunctionUtils::FunctionID> ids;
        for (const auto &func : _customCallbacks.GetFunctions())
        {
            ids.push_back(func->GetID());
        }
        for (auto id : ids)
        {
            _customCallbacks.DeleteFunction(id);
        }
    }
};

} // namespace Sound::Resource
