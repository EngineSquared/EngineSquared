#pragma once

#include "core/Core.hpp"
#include "utils/PointLight.hpp"

namespace Graphic::Resource {
class PointLightsBuffer;

struct PointLights {
    PointLights() = default;
    ~PointLights() = default;

    using ID = entt::hashed_string;
    ID GetID() const { return Utils::POINT_LIGHTS_BUFFER_ID; }

    void Create(Engine::Core &core);
    void Update(Engine::Core &core);
};
} // namespace Graphic::Resource
