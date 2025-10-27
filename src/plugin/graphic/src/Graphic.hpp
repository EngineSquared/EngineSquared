#pragma once

// Resources
#include "resource/Context.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Image.hpp"
#include "resource/Limits.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"
#include "resource/Surface.hpp"
#include "resource/Texture.hpp"

// Utils
#include "utils/IValidable.hpp"
#include "utils/shader/ABindGroupLayoutEntry.hpp"
#include "utils/shader/BindGroupLayout.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/ColorTargetState.hpp"
#include "utils/shader/DepthStencilState.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
#include "utils/shader/VertexBufferLayout.hpp"
#include "utils/GetBytesPerPixel.hpp"

// Exceptions
#include "exception/AdapterCreationError.hpp"
#include "exception/CapabilitiesRequestError.hpp"
#include "exception/DeviceCreationError.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "exception/InstanceCreationError.hpp"
#include "exception/QueueCreationError.hpp"
#include "exception/SurfaceCreationError.hpp"
#include "exception/UnknownFileError.hpp"
#include "exception/UnknownFormatType.hpp"
#include "exception/UnsetAttributeTextureDescriptor.hpp"
#include "exception/UnsupportedTextureFormatError.hpp"
#include "exception/VertexAttributeOverlappingError.hpp"

// Plugin
#include "plugin/PluginGraphic.hpp"

// Systems
#include "system/initialization/ConfigureSurface.hpp"
#include "system/initialization/CreateAdapter.hpp"
#include "system/initialization/CreateDevice.hpp"
#include "system/initialization/CreateInstance.hpp"
#include "system/initialization/CreateQueue.hpp"
#include "system/initialization/CreateSurface.hpp"
#include "system/initialization/ReleaseAdapter.hpp"
#include "system/initialization/ReleaseInstance.hpp"
#include "system/initialization/RequestCapabilities.hpp"
#include "system/initialization/SetupQueue.hpp"

#include "system/shutdown/ReleaseContext.hpp"
