#pragma once

#include <string>

#include <entt/entt.hpp>

#include "AssetsHandle.hpp"
#include "Loader.hpp"

namespace ES::Plugin::OpenGL::Component {
using ShaderHandle = ES::Plugin::Object::Component::AssetsHandle<Utils::ShaderProgram>;
} // namespace ES::Plugin::OpenGL::Component
