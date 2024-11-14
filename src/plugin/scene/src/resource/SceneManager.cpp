#include "Engine.hpp"

#include "Logger.hpp"

#include "AScene.hpp"

#include "SceneManager.hpp"

void ES::Plugin::Scene::Resource::SceneManager::Update(ES::Engine::Registry &registry)
{
    if (!_nextScene.has_value())
    {
        ES::Utils::Log::Warn("Unable to load next scene: No next scene provided");
        return;
    }
    if (_currentScene.has_value())
    {
        _unloadScene(registry, _currentScene.value());
    }
    _loadScene(registry, _nextScene.value());
    _currentScene = _nextScene;
    _nextScene.reset();
}
void ES::Plugin::Scene::Resource::SceneManager::_loadScene(ES::Engine::Registry &registry, const std::string &name)
{
    ES::Utils::Log::Info("Loading scene: " + _nextScene.value());
    std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scene = _getScene(name);
    if (scene.has_value())
    {
        scene.value()->Load(registry);
    }
}

void ES::Plugin::Scene::Resource::SceneManager::_unloadScene(ES::Engine::Registry &registry, const std::string &name)
{
    ES::Utils::Log::Info("Unloading scene: " + _currentScene.value());
    std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scene = _getScene(name);
    if (scene.has_value())
    {
        scene.value()->Unload(registry);
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
