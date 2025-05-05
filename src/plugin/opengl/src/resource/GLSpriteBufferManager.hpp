/**************************************************************************
 * Engine² v0.0.0
 *
 * This file is part of the Engine² project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * Engine² is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file GLSpriteBufferManager.hpp
 * @brief GLSpriteBufferManager class declaration using Object::Resource::ResourceManager.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-04-04
 **************************************************************************/

#pragma once

#include "GLSpriteBuffer.hpp"
#include "Object.hpp"

namespace ES::Plugin::OpenGL::Resource {

using GLSpriteBufferManager = Object::Resource::ResourceManager<Utils::GLSpriteBuffer>;

} // namespace ES::Plugin::OpenGL::Resource
