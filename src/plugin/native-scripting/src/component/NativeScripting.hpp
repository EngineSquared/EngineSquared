#pragma once

#include "ScriptableEntity.hpp"

namespace ES::Plugin::NativeScripting::Component {
/**
 * Component used to allow native scripting for entities.
 */
struct NativeScripting {
    ES::Plugin::NativeScripting::Resource::ScriptableEntity *seInstance = nullptr;

    std::function<void()> Instantiate;
    std::function<void()> DestroyInstance;

    std::function<void(ES::Plugin::NativeScripting::Resource::ScriptableEntity *)> OnCreate;
    std::function<void(ES::Plugin::NativeScripting::Resource::ScriptableEntity *)> OnDestroy;
    std::function<void(ES::Plugin::NativeScripting::Resource::ScriptableEntity *, float ts)> OnUpdate;

    template <typename T> void Bind(ES::Engine::Registry &registry)
    {
        Instantiate = [&]() { seInstance = new T(registry); };
        DestroyInstance = [&]() {
            delete (T *) seInstance;
            seInstance = nullptr;
        };

        OnCreate = [](ES::Plugin::NativeScripting::Resource::ScriptableEntity *instance) {
            ((T *) instance)->OnCreate();
        };
        OnDestroy = [](ES::Plugin::NativeScripting::Resource::ScriptableEntity *instance) {
            ((T *) instance)->OnDestroy();
        };
        OnUpdate = [](ES::Plugin::NativeScripting::Resource::ScriptableEntity *instance, float ts) {
            ((T *) instance)->OnUpdate(ts);
        };
    }
};
} // namespace ES::Plugin::NativeScripting::Component
