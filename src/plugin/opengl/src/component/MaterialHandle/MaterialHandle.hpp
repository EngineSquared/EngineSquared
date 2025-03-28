#pragma once

#include <string>

#include <entt/entt.hpp>
#include "Object.hpp"
#include "Material.hpp"

namespace ES::Plugin::OpenGL::Component {
using MaterialHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::Material>;
} // namespace ES::Plugin::OpenGL::Component
