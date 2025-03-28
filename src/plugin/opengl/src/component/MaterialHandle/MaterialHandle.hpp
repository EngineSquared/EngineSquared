#pragma once

#include <string>

#include "Material.hpp"
#include "Object.hpp"
#include <entt/entt.hpp>

namespace ES::Plugin::OpenGL::Component {
using MaterialHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::Material>;
} // namespace ES::Plugin::OpenGL::Component
