#pragma once

#include <string>

#include "AssetsHandle.hpp"
#include "Font.hpp"
#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
using FontHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::Font>;
} // namespace ES::Plugin::OpenGL::Component
