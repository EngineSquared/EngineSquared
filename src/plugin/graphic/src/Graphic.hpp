#pragma once

// Resources
#include "resource/Context.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Limits.hpp"
#include "resource/Surface.hpp"
#include "resource/Texture.hpp"
#include "resource/Image.hpp"

// Utils
#include "utils/webgpu.hpp"

// Exceptions
#include "exception/AdapterCreationError.hpp"
#include "exception/CapabilitiesRequestError.hpp"
#include "exception/DeviceCreationError.hpp"
#include "exception/InstanceCreationError.hpp"
#include "exception/QueueCreationError.hpp"
#include "exception/SurfaceCreationError.hpp"
#include "exception/UnsetAttributeTextureDescriptor.hpp"

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
