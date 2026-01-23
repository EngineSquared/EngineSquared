#pragma once

#include "utils/ScriptableEntity.hpp"

namespace NativeScripting::Component {
/**
 * Component used to allow native scripting for entities.
 * Implementation is well explained and inspired from this Youtube video made by @TheCherno
 * https://www.youtube.com/watch?v=iIUhg88MK5M&t=901s&pp=ygUbbmF0aXZlIHNjcmlwdGluZyB0aGUgY2hlcm5v
 */
struct NativeScripting {
    std::unique_ptr<Utils::ScriptableEntity> seInstance = nullptr;

    std::function<void(Engine::Entity)> Instantiate;
    std::function<void()> DestroyInstance;

    std::function<void(Utils::ScriptableEntity *)> OnCreate;
    std::function<void(Utils::ScriptableEntity *)> OnDestroy;
    std::function<void(Utils::ScriptableEntity *)> OnUpdate;

    template <typename T> void Bind(Engine::Core &core)
    {
        Instantiate = [this, &core](Engine::Entity entity) { seInstance = std::make_unique<T>(entity); };
        DestroyInstance = [this]() { seInstance.reset(); };

        OnCreate = [&core](Utils::ScriptableEntity *instance) { static_cast<T *>(instance)->OnCreate(core); };
        OnDestroy = [&core](Utils::ScriptableEntity *instance) { static_cast<T *>(instance)->OnDestroy(core); };
        OnUpdate = [&core](Utils::ScriptableEntity *instance) { static_cast<T *>(instance)->OnUpdate(core); };
    }
};
} // namespace NativeScripting::Component
