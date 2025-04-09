#include "PluginScene.hpp"
#include "Update.hpp"
#include "UpdateScene.hpp"
#include "SceneManager.hpp"

void ES::Plugin::Scene::Plugin::Bind()
{
    RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Scene::System::UpdateScene);
}
