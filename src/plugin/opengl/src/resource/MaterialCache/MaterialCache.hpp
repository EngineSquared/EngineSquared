#pragma once
#include "MaterialData.hpp"
#include "Object.hpp"
#include <map>
#include <string>

namespace ES::Plugin::OpenGL::Resource {
using MaterialCache = ES::Plugin::Object::Resource::AssetsManager<Utils::MaterialData>;
}
