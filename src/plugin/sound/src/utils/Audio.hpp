#pragma once

#include <miniaudio.h>
#include <string>

namespace ES::Plugin::Sound::Utils {
struct Audio {
    std::string name;
    std::string path;
    ma_decoder decoder;
    bool loop = false;
    bool isPlaying = false;
    bool isPaused = false;
    float volume = 1.0f;
    ma_uint64 loopStartFrame = 0ull;
    ma_uint64 loopEndFrame = 0ull;
};
} // namespace ES::Plugin::Sound::Utils
