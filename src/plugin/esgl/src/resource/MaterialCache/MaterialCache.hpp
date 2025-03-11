#pragma once
#include <map>
#include <string>
#include "Material.hpp"
#include "Object.hpp"

namespace ESGL {
    using MaterialCache = ES::Plugin::Object::Resource::AssetsManager<Material>;
}