#pragma once

// Resources
#include "resource/Context.hpp"
#include "resource/Shader.hpp"

// Utils
#include "utils/webgpu.hpp"
#include "utils/ShaderBuilder.hpp"
#include "utils/BindGroupLayout.hpp"
#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/BufferBindGroupLayoutEntry.hpp"

// Exceptions
#include "exception/InstanceCreationError.hpp"
#include "exception/VertexAttributeOverlappingError.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"

// Plugin
#include "plugin/PluginGraphic.hpp"

// Systems
#include "system/initialization/CreateInstance.hpp"
