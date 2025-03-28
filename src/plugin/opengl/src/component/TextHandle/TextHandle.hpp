#pragma once

#include <string>

#include <entt/entt.hpp>
#include "AssetsHandle.hpp"
#include "GLTextBuffer.hpp"

namespace ES::Plugin::OpenGL::Component {
using TextHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::GLTextBuffer>;
} // namespace ES::Plugin::OpenGL::Component
