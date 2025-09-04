#include "plugin/PluginScene.hpp"
#include "resource/SceneManager.hpp"
#include "scheduler/Update.hpp"
#include "system/UpdateScene.hpp"

void ES::Plugin::Scene::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Scene::System::UpdateScene);
}
