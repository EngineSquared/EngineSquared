#include "ScriptingSystem.hpp"

void ES::Plugin::NativeScripting::System::UpdateScripts(ES::Engine::Core &core)
{
    core.GetRegistry().view<ES::Plugin::NativeScripting::Component::NativeScripting>().each(
        [&core](auto entity, auto &nsComponent) {
            if (!nsComponent.seInstance.get())
            {
                nsComponent.Instantiate();
                nsComponent.seInstance->entity = entity;
                nsComponent.OnCreate(nsComponent.seInstance.get());
            }

            nsComponent.OnUpdate(nsComponent.seInstance.get());
        });
}

void ES::Plugin::NativeScripting::System::DestroyScript(entt::registry &registry, entt::entity entity)
{
    const auto &script = registry.get<ES::Plugin::NativeScripting::Component::NativeScripting>(entity);
    script.OnDestroy(script.seInstance.get());
    script.DestroyInstance();
}

void ES::Plugin::NativeScripting::System::SetOnDestroy(ES::Engine::Core &core)
{
    core.GetRegistry().on_destroy<ES::Plugin::NativeScripting::Component::NativeScripting>().connect<&DestroyScript>();
}