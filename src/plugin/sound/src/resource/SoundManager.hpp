#pragma once

#include <map>
#include <miniaudio.h>

#include "Engine.hpp"

namespace ES::Plugin::Sound::Resource {
class SoundManager {
  private:
    ma_result _result;
    ma_decoder _decoder;
    ma_device_config _deviceConfig;
    ma_device _device;

    struct Sound {
        std::string name;
        std::string path;
        ma_decoder decoder;
        bool loop = false;
        bool isPlaying = false;
        bool isPaused = false;
        float volume = 1.0f;
        ma_uint64 loopStartFrame = 0;
        ma_uint64 loopEndFrame = 0;
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
        auto *self = static_cast<SoundManager *>(pDevice->pUserData);
        auto *outputBuffer = static_cast<float *>(pOutput);
        std::memset(outputBuffer, 0, sizeof(float) * frameCount * 2); // stereo clear

        for (auto &[name, sound] : self->_soundsToPlay)
        {
            if (!sound.isPlaying || sound.isPaused)
                continue;

            std::array<float, 4096 * 2> tempBuffer; // large enough for stereo samples
            ma_uint32 framesRemaining = frameCount;
            ma_uint32 tempBufferSize = tempBuffer.size() / sizeof(float) / 2;

            while (framesRemaining > 0)
            {
                ma_uint32 framesToRead = (tempBufferSize < framesRemaining) ? tempBufferSize : framesRemaining;
                ma_uint64 framesRead = 0;
                ma_result result =
                    ma_decoder_read_pcm_frames(&sound.decoder, tempBuffer.data(), framesToRead, &framesRead);

                if (result != MA_SUCCESS)
                {
                    ES::Utils::Log::Error(fmt::format("Could not read PCM frames: {}", ma_result_description(result)));
                }

                if (framesRead < framesToRead)
                {
                    if (sound.loop)
                    {
                        ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                        continue;
                    }
                    else
                    {
                        sound.isPlaying = false;
                        break;
                    }
                }
                if (sound.loop && sound.loopEndFrame > 0)
                {
                    ma_uint64 currentFrame;
                    ma_decoder_get_cursor_in_pcm_frames(&sound.decoder, &currentFrame);
                    if (currentFrame >= sound.loopEndFrame)
                    {
                        ma_decoder_seek_to_pcm_frame(&sound.decoder, sound.loopStartFrame);
                    }
                }
                // Mix into output buffer (stereo, interleaved)
                for (ma_uint32 i = 0; i < framesRead * 2; ++i)
                {
                    outputBuffer[i] += tempBuffer[i] * sound.volume;
                }
                framesRemaining -= static_cast<ma_uint32>(framesRead);
            }
        }
    }

    /**
     * @brief Initialize the sound system.
     *
     * @return void
     */
    inline void Init()
    {
        _deviceConfig = ma_device_config_init(ma_device_type_playback);
        _deviceConfig.playback.format = ma_format_f32;
        _deviceConfig.playback.channels = 2;
        _deviceConfig.sampleRate = 44100;
        _deviceConfig.dataCallback = SoundManager::data_callback;
        _deviceConfig.pUserData = this;

        _result = ma_device_init(NULL, &_deviceConfig, &_device);
        if (_result != MA_SUCCESS)
        {
            ES::Utils::Log::Error(fmt::format("Failed to init audio device: {}", ma_result_description(_result)));
            return;
        }

        _result = ma_device_start(&_device);
        if (_result != MA_SUCCESS)
        {
            ES::Utils::Log::Error(fmt::format("Failed to start audio device: {}", ma_result_description(_result)));
            ma_device_uninit(&_device);
        }
        else
        {
            ES::Utils::Log::Info("Audio device started successfully.");
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
            ES::Utils::Log::Warn(fmt::format("Could not register: Sound \"{}\" already exists", soundName));
            return;
        }

        Sound sound;
        sound.name = soundName;
        sound.path = filePath;
        sound.loop = loop;

        _result = ma_decoder_init_file(filePath.c_str(), nullptr, &sound.decoder);
        if (_result != MA_SUCCESS)
        {
            ES::Utils::Log::Error(
                fmt::format("Failed to initialize the audio device: {}", ma_result_description(_result)));
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
        auto it = _soundsToPlay.find(soundName);
        if (it != _soundsToPlay.end())
        {
            ma_decoder_uninit(&it->second.decoder);
            _soundsToPlay.erase(it);
        }
        else
        {
            ES::Utils::Log::Error(fmt::format("Could not unregister: Sound \"{}\" does not exist", soundName));
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
            it->second.isPlaying = true;
            it->second.isPaused = false;
        }
        else
        {
            ES::Utils::Log::Error(fmt::format("Could not play: Sound \"{}\" does not exist", soundName));
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
            it->second.isPlaying = false;
            it->second.isPaused = false;
            ma_decoder_seek_to_pcm_frame(&it->second.decoder, 0);
        }
        else
        {
            ES::Utils::Log::Error(fmt::format("Could not stop: Sound \"{}\" does not exist", soundName));
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
            it->second.isPaused = true;
        }
        else
        {
            ES::Utils::Log::Error(fmt::format("Could not pause: Sound \"{}\" does not exist", soundName));
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
        ES::Utils::Log::Error(fmt::format("Could not verify playing status: Sound \"{}\" does not exist", soundName));
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
            ES::Utils::Log::Error(fmt::format("Could not set volume: Sound \"{}\" does not exist", soundName));
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
            ES::Utils::Log::Error(fmt::format("Could not set loop: Sound \"{}\" does not exist", soundName));
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
                ES::Utils::Log::Error(
                    fmt::format("Something went wrong while computing PCM frames length of sound \"{}\"", soundName));
                return;
            }

            ma_uint64 startFrame = static_cast<ma_uint64>(startSeconds * sampleRate);
            ma_uint64 endFrame = (endSeconds > 0.0f) ? static_cast<ma_uint64>(endSeconds * sampleRate) : totalFrames;

            if (startFrame >= totalFrames || endFrame > totalFrames || startFrame >= endFrame)
            {
                ES::Utils::Log::Warn(fmt::format("Invalid loop range for \"{}\": {}s to {}s, ignored", soundName,
                                                 startSeconds, endSeconds));
                return;
            }
            it->second.loopStartFrame = startFrame;
            it->second.loopEndFrame = endFrame;
        }
        else
        {
            ES::Utils::Log::Error(fmt::format("Could not set loop points: Sound \"{}\" does not exist", soundName));
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
            ES::Utils::Log::Error(
                fmt::format("Could not get the playback position: Sound \"{}\" does not exist", soundName));
            return -1.0f;
        }

        Sound &sound = it->second;
        if (!sound.isPlaying)
        {
            ES::Utils::Log::Warn(fmt::format("Sound \"{}\" is not currently playing", soundName));
            return 0.0f;
        }

        ma_uint64 currentFrame;
        _result = ma_decoder_get_cursor_in_pcm_frames(&sound.decoder, &currentFrame);
        if (_result != MA_SUCCESS)
        {
            ES::Utils::Log::Error(
                fmt::format("Could not get the playback position: {}", ma_result_description(_result)));
            return -1.0f;
        }
        double position = static_cast<double>(currentFrame) / sound.decoder.outputSampleRate;
        return position;
    }
};
} // namespace ES::Plugin::Sound::Resource
