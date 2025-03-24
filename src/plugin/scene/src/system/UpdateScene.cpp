#include "UpdateScene.hpp"

#include "SceneManager.hpp"

void ES::Plugin::Scene::System::UpdateScene(ES::Engine::Core &core)
{
    core.GetResource<ES::Plugin::Scene::Resource::SceneManager>().Update(core);
}
