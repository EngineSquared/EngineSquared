#pragma once

#include "ScriptableEntity.hpp"

namespace ES::Plugin::NativeScripting::Component {
/**
 * Component used to allow native scripting for entities.
 */
struct NativeScripting {
    ES::Plugin::NativeScripting::Utils::ScriptableEntity *seInstance = nullptr;

    std::function<void()> Instantiate;
    std::function<void()> DestroyInstance;

    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnCreate;
    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnDestroy;
    std::function<void(ES::Plugin::NativeScripting::Utils::ScriptableEntity *)> OnUpdate;

    template <typename T> void Bind(ES::Engine::Registry &registry)
    {
        Instantiate = [&]() { seInstance = new T(); };
        DestroyInstance = [&]() {
            delete (T *) seInstance;
            seInstance = nullptr;
        };

        OnCreate = [&registry](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            ((T *) instance)->OnCreate(registry);
        };
        OnDestroy = [](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            ((T *) instance)->OnDestroy();
        };
        OnUpdate = [&registry](ES::Plugin::NativeScripting::Utils::ScriptableEntity *instance) {
            ((T *) instance)->OnUpdate(registry);
        };
    }
};
} // namespace ES::Plugin::NativeScripting::Component
