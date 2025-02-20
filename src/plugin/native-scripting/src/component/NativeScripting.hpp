#pragma once

#include "ScriptableEntity.hpp"

namespace ES::Plugin::NativeScripting::Component {
/**
 * Component used to allow native scripting for entities.
 * Implementation is well explained and inspired from this Youtube video made by @TheCherno
 * https://www.youtube.com/watch?v=iIUhg88MK5M&t=901s&pp=ygUbbmF0aXZlIHNjcmlwdGluZyB0aGUgY2hlcm5v
 */
struct NativeScripting {
    std::unique_ptr<ES::Plugin::NativeScripting::Utils::ScriptableEntity> seInstance = nullptr;

    std::function<void()> Instantiate;
    std::function<void()> DestroyInstance;

    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnCreate;
    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnDestroy;
    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnUpdate;

    template <typename T> void Bind(ES::Engine::Registry &registry)
    {
        Instantiate = [this]() { seInstance = std::make_unique<T>(); };
        DestroyInstance = [this]() { seInstance.reset(); };

        OnCreate = [&registry](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            static_cast<T *>(instance)->OnCreate(registry);
        };
        OnDestroy = [](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            static_cast<T *>(instance)->OnDestroy();
        };
        OnUpdate = [&registry](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            static_cast<T *>(instance)->OnUpdate(registry);
        };
    }
};
} // namespace ES::Plugin::NativeScripting::Component
