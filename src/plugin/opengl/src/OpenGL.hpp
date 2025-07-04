#pragma once

// Components
#include "component/CubeMapHandle.hpp"
#include "component/FontHandle.hpp"
#include "component/Light.hpp"
#include "component/MaterialHandle.hpp"
#include "component/ModelHandle.hpp"
#include "component/ShaderHandle.hpp"
#include "component/Sprite.hpp"
#include "component/SpriteHandle.hpp"
#include "component/Text.hpp"
#include "component/TextHandle.hpp"
#include "component/TextureHandle.hpp"

// Exception
#include "exception/OpenGLError.hpp"

// Plugin
#include "plugin/PluginOpenGL.hpp"

// Resources
#include "resource/Camera.hpp"
#include "resource/CubeMapManager.hpp"
#include "resource/DirectionalLight.hpp"
#include "resource/FontManager.hpp"
#include "resource/GLMeshBufferManager.hpp"
#include "resource/GLSpriteBufferManager.hpp"
#include "resource/GLTextBufferManager.hpp"
#include "resource/MaterialCache.hpp"
#include "resource/MouseDragging.hpp"
#include "resource/ShaderManager.hpp"
#include "resource/TextureManager.hpp"

// Systems
#include "system/BufferSystems.hpp"
#include "system/ManagerSystems.hpp"
#include "system/RenderSystems.hpp"
#include "system/ShaderSystems.hpp"
#include "system/WindowSystems.hpp"

// Utils
#include "utils/CreateSkyBox.hpp"
#include "utils/CubeMap.hpp"
#include "utils/Font.hpp"
#include "utils/GLMeshBuffer.hpp"
#include "utils/GLTextBuffer.hpp"
#include "utils/LightInfo.hpp"
#include "utils/Loader.hpp"
#include "utils/Material.hpp"
#include "utils/Texture.hpp"
#include "utils/Viewer.hpp"
