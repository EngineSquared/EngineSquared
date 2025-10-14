#include "plugin/PluginScene.hpp"
#include "resource/SceneManager.hpp"
#include "scheduler/Update.hpp"
#include "system/UpdateScene.hpp"

void Scene::Plugin::Bind()
{
    RegisterResource<Resource::SceneManager>(Resource::SceneManager());
    RegisterSystems<Engine::Scheduler::Update>(System::UpdateScene);
}
