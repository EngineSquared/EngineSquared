#pragma once

#include "core/Core.hpp"
#include "entity/Entity.hpp"

namespace DefaultPipeline::System {

void OnMeshDestruction(Engine::Core &core, Engine::EntityId entityId);

} // namespace DefaultPipeline::System
