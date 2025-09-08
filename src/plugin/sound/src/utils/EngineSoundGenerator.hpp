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
 * @file EngineSoundGenerator.hpp
 * @brief EngineSoundGenerator struct declaration.
 *
 * This functions are used to generate engine sound data based on engine parameters
 * such as RPM, strokes, and cylinders.
 *
 * Based on the work of jgardner8 on https://github.com/jgardner8/engine-sound-simulator
 *
 * @author @MasterLaplace
 * @version 0.1.0
 * @date 2025-09-07
 **************************************************************************/

#pragma once

#include "Engine.hpp"
#include "EngineSoundData.hpp"

#include <algorithm>
#include <chrono>
#include <math.h>
#include <random>
#include <miniaudio.h>

namespace ES::Plugin::Sound::Utils {

// Audio configuration constants
constexpr float AMPLITUDE = 0.15f;
constexpr float PI = 3.14159265359f;

void PrepareEngineBuffers(Component::EngineSoundData *data, ma_uint32 sample_rate);

float MixCylinders(Component::EngineSoundData *data, float time_seconds);

void EngineDataCallback(ma_device *pDevice, void *pOutput, ma_uint32 frameCount);

} // namespace ES::Plugin::Sound::Utils
