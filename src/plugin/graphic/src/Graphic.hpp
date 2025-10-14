#pragma once

// Resources
#include "resource/Context.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"

// Utils
#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/BindGroupLayout.hpp"
#include "utils/BufferBindGroupLayoutEntry.hpp"
#include "utils/ColorTargetState.hpp"
#include "utils/DepthStencilState.hpp"
#include "utils/IValidable.hpp"
#include "utils/SamplerBindGroupLayoutEntry.hpp"
#include "utils/TextureBindGroupLayoutEntry.hpp"
#include "utils/VertexBufferLayout.hpp"

// Exceptions
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "exception/UnknownFormatType.hpp"

// Plugin
#include "plugin/PluginGraphic.hpp"

// Systems
#include "system/initialization/CreateInstance.hpp"
