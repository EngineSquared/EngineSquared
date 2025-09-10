#include "Engine.hpp"

#include "Logger.hpp"

#include "utils/AScene.hpp"

#include "resource/SceneManager.hpp"

void ES::Plugin::Scene::Resource::SceneManager::Update(ES::Engine::Core &core)
{
    if (!_nextScene.has_value())
    {
        return;
    }
    if (_currentScene.has_value())
    {
        _unloadScene(core, _currentScene.value());
    }
    _loadScene(core, _nextScene.value());
    _currentScene = _nextScene;
    _nextScene.reset();
}
void ES::Plugin::Scene::Resource::SceneManager::_loadScene(ES::Engine::Core &core, const std::string &name)
{
    ES::Utils::Log::Info("Loading scene: " + _nextScene.value());
    std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scene = _getScene(name);
    if (scene.has_value())
    {
        scene.value()->Load(core);
    }
}

void ES::Plugin::Scene::Resource::SceneManager::_unloadScene(ES::Engine::Core &core, const std::string &name)
{
    ES::Utils::Log::Info("Unloading scene: " + _currentScene.value());
    std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scene = _getScene(name);
    if (scene.has_value())
    {
        scene.value()->Unload(core);
    }
}

std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>>
ES::Plugin::Scene::Resource::SceneManager::_getScene(const std::string &name)
{
    auto scene = _scenes.find(name);
    if (scene != _scenes.end())
    {
        return scene->second;
    }
    else
    {
        ES::Utils::Log::Error("Scene not found: " + name);
        return std::nullopt;
    }
}
