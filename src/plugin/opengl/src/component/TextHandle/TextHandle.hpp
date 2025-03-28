#pragma once

#include <string>

#include "AssetsHandle.hpp"
#include "GLTextBuffer.hpp"
#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
using TextHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::GLTextBuffer>;
} // namespace ES::Plugin::OpenGL::Component
