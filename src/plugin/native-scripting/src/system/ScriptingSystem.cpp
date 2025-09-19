#include "ScriptingSystem.hpp"

void Plugin::NativeScripting::System::UpdateScripts(Engine::Core &core)
{
    core.GetRegistry().view<Component::NativeScripting>().each([](auto entity, auto &nsComponent) {
        if (!nsComponent.seInstance.get())
        {
            nsComponent.Instantiate();
            nsComponent.seInstance->entity = entity;
            nsComponent.OnCreate(nsComponent.seInstance.get());
        }

        nsComponent.OnUpdate(nsComponent.seInstance.get());
    });
}

void Plugin::NativeScripting::System::DestroyScript(entt::registry &registry, entt::entity entity)
{
    const auto &script = registry.get<Component::NativeScripting>(entity);
    script.OnDestroy(script.seInstance.get());
    script.DestroyInstance();
}

void Plugin::NativeScripting::System::SetOnDestroy(Engine::Core &core)
{
    core.GetRegistry().on_destroy<Component::NativeScripting>().connect<&DestroyScript>();
}
