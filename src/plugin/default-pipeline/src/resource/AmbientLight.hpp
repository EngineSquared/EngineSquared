#pragma once

#include "core/Core.hpp"
#include "utils/AmbientLight.hpp"

namespace DefaultPipeline::Resource {
class AmbientLightBuffer;

struct AmbientLight {
    AmbientLight() = default;
    ~AmbientLight() = default;

    using ID = entt::hashed_string;
    ID GetID() const { return Utils::AMBIENT_LIGHT_BUFFER_ID; }

    void Create(Engine::Core &core);
    void Update(Engine::Core &core);
    void SetEntity(Engine::Core &core, Engine::Entity entity);
    void SetValue(Engine::Core &core, const Object::Component::AmbientLight &ambientLight);

  private:
    AmbientLightBuffer &GetLight(Engine::Core &core);
    Object::Component::AmbientLight _ambientLightComponent{};
};
} // namespace Graphic::Resource
