#pragma once

#include <string>

#include "AssetsHandle.hpp"
#include "GLMeshBuffer.hpp"
#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
using ModelHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::GLMeshBuffer>;
} // namespace ES::Plugin::OpenGL::Component
