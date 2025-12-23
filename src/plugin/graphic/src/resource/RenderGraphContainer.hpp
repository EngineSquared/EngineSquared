#pragma once

#include "resource/RenderGraph.hpp"
#include "resource/ResourceManager.hpp"

namespace Graphic::Resource {

/**
 * @brief RenderGraphContainer is a resource that stores RenderGraph resources.
 *
 * This class provides a convenient way to manage multiple render graphs in the graphics system.
 * It uses the ResourceManager template to store and retrieve render graphs by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::RenderGraph
 */
using RenderGraphContainer = Object::Resource::ResourceManager<RenderGraph>;

} // namespace Graphic::Resource
