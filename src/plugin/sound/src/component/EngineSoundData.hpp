/**************************************************************************
 * EngineSquared v0.1.0
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2025 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file EngineSoundData.hpp
 * @brief EngineSoundData struct declaration.
 *
 * This struct is used to represent engine sound data.
 *
 * @author @MasterLaplace
 * @version 0.1.0
 * @date 2025-09-07
 **************************************************************************/

#pragma once

#include <vector>

namespace ES::Plugin::Sound::Component {

struct EngineSoundData {
    float t = 0.0f;
    float rpm = 900.0f;
    int strokes = 4;
    int cylinders = 6;
    std::vector<float> cylinder_timings = {0.0f, 120.0f, 240.0f, 360.0f, 480.0f, 600.0f};

    float strokes_per_sec = (rpm * 2.0f) / 60.0f;
    float sec_between_fires = strokes_per_sec / strokes;
    float fire_duration = sec_between_fires / strokes;
    float cycle_duration = sec_between_fires;
    float cycle_position = 0.0f;

    std::vector<float> fire_sound;
    std::vector<float> silence_sound;

    std::vector<std::vector<float>> cylinder_buffers;
    int buffer_size = 0;
    bool buffers_ready = false;

    EngineSoundData() = default;
    ~EngineSoundData() = default;
};

} // namespace ES::Plugin::Sound::Component
