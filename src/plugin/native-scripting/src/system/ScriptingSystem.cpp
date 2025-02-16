#include "ScriptingSystem.hpp"

void ES::Plugin::NativeScripting::System::UpdateScripts(ES::Engine::Registry &registry)
{
    registry.GetRegistry().view<ES::Plugin::NativeScripting::Component::NativeScripting>().each(
        [=](auto entity, auto &nsComponent) {
            if (!nsComponent.seInstance)
            {
                nsComponent.Instantiate();
                nsComponent.seInstance->SetEntity(entity);
                nsComponent.OnCreate(nsComponent.seInstance);
            }

            nsComponent.OnUpdate(nsComponent.seInstance, 0.0f);
        });
}
