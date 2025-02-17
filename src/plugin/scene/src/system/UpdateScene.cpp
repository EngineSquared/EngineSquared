#include "UpdateScene.hpp"

#include "SceneManager.hpp"

void ES::Plugin::Scene::System::UpdateScene(ES::Engine::Core &reg)
{
    reg.GetResource<ES::Plugin::Scene::Resource::SceneManager>().Update(reg);
}
