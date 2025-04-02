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
 * @file GLMeshBufferManager.hpp
 * @brief GLMeshBufferManager class declaration using Object::Resource::ResourceManager.
 *
 * @author @EngineSquared Team
 * @version 0.0.0
 * @date 2025-03-21
 **************************************************************************/

#pragma once

#include "GLMeshBuffer.hpp"
#include "Object.hpp"

namespace ES::Plugin::OpenGL::Resource {

using GLMeshBufferManager = Object::Resource::ResourceManager<Utils::GLMeshBuffer>;

} // namespace ES::Plugin::OpenGL::Resource
