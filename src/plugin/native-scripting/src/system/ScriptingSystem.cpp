#include "ScriptingSystem.hpp"

void ES::Plugin::ScriptingSystem::System::UpdateScripts(ES::Engine::Registry &reg)
{
    reg.GetRegistry().view<ES::Plugin::NativeScripting::Component::NativeScripting>().each([=](auto entity, auto &nsComponent)
    {
        if (!nsComponent.seInstance)
        {
            nsComponent.Instantiate();
            nsComponent.seInstance->entity = entity;
            nsComponent.OnCreate(nsComponent.seInstance);
        }

        nsComponent.OnUpdate(nsComponent.seInstance, 0.0f);
    });
}