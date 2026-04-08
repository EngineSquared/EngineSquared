#pragma once

#include "resource/ResourceManager.hpp"
#include "utils/webgpu.hpp"
#include "resource/TextureView.hpp"

namespace Graphic::Resource {

using TextureViewContainer = Object::Resource::ResourceManager<Resource::TextureView>;

}
