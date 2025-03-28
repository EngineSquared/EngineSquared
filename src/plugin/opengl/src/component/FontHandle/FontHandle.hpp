#pragma once

#include <string>

#include <entt/entt.hpp>
#include "AssetsHandle.hpp"
#include "Font.hpp"

namespace ES::Plugin::OpenGL::Component {
using FontHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::Font>;
} // namespace ES::Plugin::OpenGL::Component
