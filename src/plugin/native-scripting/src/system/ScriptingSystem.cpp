#include "ScriptingSystem.hpp"

void ES::Plugin::NativeScripting::System::UpdateScripts(ES::Engine::Registry &registry)
{
    registry.GetRegistry().view<ES::Plugin::NativeScripting::Component::NativeScripting>().each(
        [&registry](auto entity, auto &nsComponent) {
            if (!nsComponent.seInstance.get())
            {
                nsComponent.Instantiate();
                nsComponent.seInstance->entity = entity;
                nsComponent.OnCreate(nsComponent.seInstance.get());
            }

            nsComponent.OnUpdate(nsComponent.seInstance.get());
        });
}
