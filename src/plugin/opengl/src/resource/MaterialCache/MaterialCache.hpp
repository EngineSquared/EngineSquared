#pragma once
#include "Material.hpp"
#include "Object.hpp"
#include <map>
#include <string>

namespace ES::Plugin::OpenGL::Resource {
using MaterialCache = ES::Plugin::Object::Resource::ResourceManager<Utils::Material>;
}
