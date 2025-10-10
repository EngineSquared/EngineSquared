#pragma once

// Resources
#include "resource/Context.hpp"
#include "resource/Shader.hpp"

// Utils
#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/BindGroupLayout.hpp"
#include "utils/BufferBindGroupLayoutEntry.hpp"
#include "utils/ShaderBuilder.hpp"
#include "utils/webgpu.hpp"

// Exceptions
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "exception/InstanceCreationError.hpp"
#include "exception/VertexAttributeOverlappingError.hpp"

// Plugin
#include "plugin/PluginGraphic.hpp"

// Systems
#include "system/initialization/CreateInstance.hpp"
