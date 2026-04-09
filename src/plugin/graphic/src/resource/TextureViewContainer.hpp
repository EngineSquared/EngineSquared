#pragma once

#include "resource/ResourceManager.hpp"
#include "resource/TextureView.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Resource {

using TextureViewContainer = Object::Resource::ResourceManager<Resource::TextureView>;

}
