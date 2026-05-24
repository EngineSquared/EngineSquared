#pragma once

#include "core/Core.hpp"
#include "entity/Entity.hpp"

namespace DefaultPipeline::System {
void OnMaterialUpdate(Engine::Core &core, Engine::EntityId entityId);
} // namespace DefaultPipeline::System
