#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <miniaudio.h>
#include <string_view>
#include <unordered_map>

#include "Engine.hpp"
#include "FunctionContainer.hpp"

namespace Sound::Resource {

using CustomDataCallback = std::function<void(ma_device *pDevice, void *pOutput, ma_uint32 frameCount)>;

class SoundManager {
  private:
    ma_result _result;
    ma_device_config _deviceConfig;
    ma_device _device;
    bool _deviceInit = false;
    ma_engine _engine;
    bool _engineInit = false;

    // Deferred error reporting for audio callback (avoid logging in real-time callback)
    // Bit flags: 0x1 = frame count too large, 0x2 = decoder read error, 0x4 = unknown format
    std::atomic<uint32_t> _callbackErrorFlags{0};
    static constexpr uint32_t ERROR_FRAME_TOO_LARGE = 0x1;
    static constexpr uint32_t ERROR_DECODER_READ = 0x2;
    static constexpr uint32_t ERROR_UNKNOWN_FORMAT = 0x4;

    struct Sound {
        std::string name;
        std::string path;
        ma_decoder decoder;
        ma_sound engineSound;
        bool loop = false;
        bool isPlaying = false;
        bool isPaused = false;
        bool hasEngineSound = false;
        bool usingEngine = false;
        float volume = 1.0f;
        ma_uint64 loopStartFrame = 0;
        ma_uint64 loopEndFrame = 0;
        bool decoderInitialized = false;
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

    /**
     * Deleted copy constructor (contains non-copyable FunctionContainer).
     */
    SoundManager(const SoundManager &) = delete;

    /**
     * Deleted copy assignment operator (contains non-copyable FunctionContainer).
     */
    SoundManager &operator=(const SoundManager &) = delete;

    /**
     * Move constructor.
     * @note std::atomic is not movable, so we manually transfer its value.
     */
    SoundManager(SoundManager &&other) noexcept
        : _result(other._result), _deviceConfig(other._deviceConfig), _device(other._device),
          _deviceInit(other._deviceInit), _engine(other._engine), _engineInit(other._engineInit),
          _callbackErrorFlags(other._callbackErrorFlags.load(std::memory_order_relaxed)),
          _soundsToPlay(std::move(other._soundsToPlay)), _customCallbacks(std::move(other._customCallbacks))
    {
        other._deviceInit = false;
        other._engineInit = false;
    }

    /**
     * Move assignment operator.
     * @note std::atomic is not movable, so we manually transfer its value.
     */
    SoundManager &operator=(SoundManager &&other) noexcept
    {
        if (this != &other)
        {
            for (auto &[name, snd] : _soundsToPlay)
            {
                if (snd.hasEngineSound)
                    ma_sound_uninit(&snd.engineSound);
                if (snd.decoderInitialized)
                    ma_decoder_uninit(&snd.decoder);
            }
            _soundsToPlay.clear();

            if (_deviceInit)
            {
                ma_device_stop(&_device);
                ma_device_uninit(&_device);
                _deviceInit = false;
            }
            if (_engineInit)
            {
                ma_engine_uninit(&_engine);
                _engineInit = false;
            }
            _result = other._result;
            _deviceConfig = other._deviceConfig;
            _device = other._device;
            _deviceInit = other._deviceInit;
            _engine = other._engine;
            _engineInit = other._engineInit;
            _callbackErrorFlags.store(other._callbackErrorFlags.load(std::memory_order_relaxed),
                                      std::memory_order_relaxed);
            _soundsToPlay = std::move(other._soundsToPlay);
            _customCallbacks = std::move(other._customCallbacks);
            other._deviceInit = false;
            other._engineInit = false;
        }
        return *this;
    }

    /**
     * @brief Audio data callback used by the playback device to fill the output buffer.
     *
     * @param pDevice Pointer to the audio playback device.
     * @param pOutput Pointer to the output buffer that will be filled with audio samples.
     * @param pInput  Unused input buffer (typically nullptr for playback-only).
     * @param frameCount Number of audio frames that must be written to the output buffer.
     *
     * @note Handles looping with configurable start/end points, checking loop bounds before reading.
     * @details Custom callbacks are invoked first, then all playing sounds are mixed. The mix buffer
     *          is cleared initially and all sounds are mixed with their respective volumes. Format
     *          conversion supports f32, s32, s16, and u8 with saturated addition to prevent overflow.
     *
     * @return void
     */
    static void data_callback(ma_device *pDevice, void *pOutput, const void * /*pInput*/, ma_uint32 frameCount)
    {
        auto *core = static_cast<Engine::Core *>(pDevice->pUserData);
        auto &self = core->GetResource<SoundManager>();
        const ma_uint32 channels = pDevice->playback.channels;
        const ma_uint32 totalSamples = frameCount * channels;

        std::memset(pOutput, 0, ma_get_bytes_per_frame(pDevice->playback.format, channels) * frameCount);

        const auto &callbacks = self._customCallbacks.GetFunctions();
        for (const auto &callback : callbacks)
        {
            (*callback)(pDevice, pOutput, frameCount);
        }

        std::array<float, 4096 * 2> mixBuffer{};
        if (totalSamples > mixBuffer.size())
        {
            self._callbackErrorFlags.fetch_or(ERROR_FRAME_TOO_LARGE, std::memory_order_relaxed);
            return;
        }

        for (auto &[name, sound] : self._soundsToPlay)
        {
            if (sound.usingEngine || !sound.isPlaying || sound.isPaused || !sound.decoderInitialized)
                continue;

            std::array<float, 4096 * 2> tempBuffer{};
            ma_uint64 framesRead = 0;

            ma_result result = ma_decoder_read_pcm_frames(&sound.decoder, tempBuffer.data(), frameCount, &framesRead);

            if (result != MA_SUCCESS && result != MA_AT_END)
            {
                self._callbackErrorFlags.fetch_or(ERROR_DECODER_READ, std::memory_order_relaxed);
            }

            for (ma_uint32 i = 0; i < framesRead * channels; ++i)
            {
                mixBuffer[i] += tempBuffer[i] * sound.volume;
            }

            if (framesRead < frameCount)
            {
                if (sound.loop)
                {
                    ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                }
                else if (result == MA_AT_END)
                {
                    sound.isPlaying = false;
                }
            }

            if (sound.loop && sound.loopEndFrame > 0)
            {
                ma_uint64 currentFrame = 0;
                ma_decoder_get_cursor_in_pcm_frames(&sound.decoder, &currentFrame);
                if (currentFrame >= sound.loopEndFrame)
                {
                    ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                }
            }
        }

        switch (pDevice->playback.format)
        {
        case ma_format_f32: {
            auto *output = static_cast<float *>(pOutput);
            for (ma_uint32 i = 0; i < totalSamples; ++i)
            {
                output[i] += std::clamp(mixBuffer[i], -1.0f, 1.0f);
            }
        }
        break;
        case ma_format_s32: {
            auto *output = static_cast<int32_t *>(pOutput);
            for (ma_uint32 i = 0; i < totalSamples; ++i)
            {
                float sample = std::clamp(mixBuffer[i], -1.0f, 1.0f);
                const int64_t mixed = static_cast<int64_t>(output[i]) + static_cast<int64_t>(sample * 2147483647.0f);
                output[i] = static_cast<int32_t>(std::clamp<int64_t>(mixed, INT32_MIN, INT32_MAX));
            }
        }
        break;
        case ma_format_s16: {
            auto *output = static_cast<int16_t *>(pOutput);
            for (ma_uint32 i = 0; i < totalSamples; ++i)
            {
                float sample = std::clamp(mixBuffer[i], -1.0f, 1.0f);
                const int32_t mixed = static_cast<int32_t>(output[i]) + static_cast<int32_t>(sample * 32767.0f);
                output[i] = static_cast<int16_t>(std::clamp<int32_t>(mixed, INT16_MIN, INT16_MAX));
            }
        }
        break;
        case ma_format_u8: {
            auto *output = static_cast<uint8_t *>(pOutput);
            for (ma_uint32 i = 0; i < totalSamples; ++i)
            {
                float sample = std::clamp(mixBuffer[i], -1.0f, 1.0f);
                const int mixed = static_cast<int>(output[i]) + static_cast<int>(sample * 127.5f);
                output[i] = static_cast<uint8_t>(std::clamp(mixed, 0, 255));
            }
        }
        break;
        default: self._callbackErrorFlags.fetch_or(ERROR_UNKNOWN_FORMAT, std::memory_order_relaxed); break;
        }
    }

    /**
     * @brief Initialize the sound system.
     *
     * @return void
     */
    inline void Init(Engine::Core &core)
    {
        _deviceConfig = ma_device_config_init(ma_device_type_playback);
        _deviceConfig.playback.format = ma_format_unknown;
        _deviceConfig.playback.channels = 2;
        _deviceConfig.sampleRate = 44100;
        _deviceConfig.dataCallback = SoundManager::data_callback;
        _deviceConfig.pUserData = &core;

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
            _deviceInit = true;
            Log::Info(fmt::format("[Audio] Device started successfully. Device format={}, sample rate={}, channels={}",
                                  static_cast<int>(_device.playback.format), _device.sampleRate,
                                  _device.playback.channels));
        }
    }

    /**
     * @brief Check and log any deferred errors from the audio callback.
     *
     * Call this method periodically from the main thread (e.g., in an Update system)
     * to report errors that occurred in the real-time audio callback without blocking it.
     */
    inline void CheckCallbackErrors()
    {
        uint32_t errors = _callbackErrorFlags.exchange(0, std::memory_order_relaxed);
        if (errors & ERROR_FRAME_TOO_LARGE)
        {
            Log::Error("[Audio] Frame count too large for mix buffer");
        }
        if (errors & ERROR_DECODER_READ)
        {
            Log::Error("[Audio] Decoder read error occurred during playback");
        }
        if (errors & ERROR_UNKNOWN_FORMAT)
        {
            Log::Error("[Audio] Unknown audio format encountered during playback");
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
        if (_soundsToPlay.contains(soundName))
        {
            Log::Warn(fmt::format("Could not register: Sound \"{}\" already exists", soundName));
            return;
        }

        Sound sound;
        sound.name = soundName;
        sound.path = filePath;
        sound.loop = loop;
        sound.decoderInitialized = false;
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            if (it->second.hasEngineSound)
                ma_sound_uninit(&it->second.engineSound);
            if (it->second.decoderInitialized)
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;

            if (!snd.usingEngine && !snd.decoderInitialized)
            {
                ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, 44100);
                _result = ma_decoder_init_file(snd.path.c_str(), &decoderConfig, &snd.decoder);
                if (_result != MA_SUCCESS)
                {
                    Log::Error(fmt::format("Failed to initialize the audio decoder for '{}': {}", snd.name,
                                           ma_result_description(_result)));
                    return;
                }
                snd.decoderInitialized = true;
            }

            snd.isPlaying = true;
            snd.isPaused = false;

            if (snd.usingEngine && snd.hasEngineSound)
            {
                ma_sound_start(&snd.engineSound);
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            snd.isPlaying = false;
            snd.isPaused = false;

            if (snd.usingEngine && snd.hasEngineSound)
            {
                ma_sound_stop(&snd.engineSound);
                ma_sound_seek_to_pcm_frame(&snd.engineSound, 0);
                if (snd.decoderInitialized)
                    ma_decoder_seek_to_pcm_frame(&snd.decoder, 0);
            }
            else
            {
                if (snd.decoderInitialized)
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            snd.isPaused = true;

            if (snd.usingEngine && snd.hasEngineSound)
            {
                ma_sound_stop(&snd.engineSound);
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            return it->second.isPlaying && !it->second.isPaused;
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            it->second.volume = std::clamp(volume, 0.0f, 1.0f);
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
     */
    inline void SetPitch(const std::string &soundName, float pitch)
    {
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            auto &snd = it->second;
            if (pitch <= 0.01f)
                pitch = 0.01f;

            if (!_engineInit)
            {
                ma_engine_config cfg = ma_engine_config_init();
                cfg.sampleRate = _device.sampleRate;
                ma_result r = ma_engine_init(&cfg, &_engine);
                if (r != MA_SUCCESS)
                {
                    Log::Error(fmt::format("Failed to init ma_engine: {}", ma_result_description(r)));
                    return;
                }
                _engineInit = true;
            }

            if (!snd.hasEngineSound)
            {
                ma_result r = ma_sound_init_from_file(&_engine, snd.path.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL,
                                                      &snd.engineSound);
                if (r != MA_SUCCESS)
                {
                    Log::Error(fmt::format("Failed to init ma_sound for '{}': {}", snd.name, ma_result_description(r)));
                    return;
                }
                snd.hasEngineSound = true;
                snd.usingEngine = true;
                ma_sound_set_looping(&snd.engineSound, snd.loop ? MA_TRUE : MA_FALSE);
                ma_sound_set_volume(&snd.engineSound, snd.volume);

                ma_uint64 cursor = 0u;
                if (snd.decoderInitialized)
                {
                    ma_decoder_get_cursor_in_pcm_frames(&snd.decoder, &cursor);
                }
                ma_sound_seek_to_pcm_frame(&snd.engineSound, cursor);

                if (snd.isPlaying && !snd.isPaused)
                {
                    ma_sound_start(&snd.engineSound);
                }
            }

            if (snd.hasEngineSound)
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
        auto it = _soundsToPlay.find(soundName);
        if (it == _soundsToPlay.end())
        {
            Log::Error(fmt::format("Could not get the playback position: Sound \"{}\" does not exist", soundName));
            return -1.0f;
        }

        Sound &sound = it->second;
        if (!sound.isPlaying)
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
