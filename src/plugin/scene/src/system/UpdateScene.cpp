#include "system/UpdateScene.hpp"

#include "resource/SceneManager.hpp"

void Plugin::Scene::System::UpdateScene(Engine::Core &core)
{
    core.GetResource<Resource::SceneManager>().Update(core);
}
