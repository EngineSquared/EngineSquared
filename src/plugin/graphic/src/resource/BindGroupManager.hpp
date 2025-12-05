#pragma once

#include "resource/BindGroup.hpp"
#include "resource/ResourceManager.hpp"

namespace Graphic::Resource {

/**
 * @brief BindGroupManager is a resource that stores BindGroup resources.
 *
 * This class provides a convenient way to manage multiple BindGroups in the graphics system.
 * It uses the ResourceManager template to store and retrieve BindGroups by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::BindGroup
 */
using BindGroupManager = Object::Resource::ResourceManager<BindGroup>;

} // namespace Graphic::Resource
