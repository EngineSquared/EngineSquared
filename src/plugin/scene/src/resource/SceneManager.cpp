#include "Engine.hpp"

#include "AScene.hpp"

#include "SceneManager.hpp"

void ES::Plugin::Scene::Resource::SceneManager::Update(ES::Engine::Registry &registry)
{
    if (!_nextScene.has_value())
    {
        std::cout
            << "[WARNING] ES::Plugin::Scene::Resource::SceneManager: Unable to load next scene: No next scene provided"
            << std::endl;
        return;
    }
    if (_currentScene.has_value())
    {
        std::cout << "[INFO] ES::Plugin::Scene::Resource::SceneManager: Unloading scene: " << _currentScene.value()
                  << std::endl;
        _unloadScene(registry, _currentScene.value());
    }
    std::cout << "[INFO] ES::Plugin::Scene::Resource::SceneManager: Loading scene: " << _nextScene.value() << std::endl;
    _loadScene(registry, _nextScene.value());
    _currentScene = _nextScene;
    _nextScene.reset();
}
void ES::Plugin::Scene::Resource::SceneManager::_loadScene(ES::Engine::Registry &registry, const std::string &name)
{
    std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scene = _getScene(name);
    if (scene.has_value())
    {
        scene.value()->Load(registry);
    }
}

void ES::Plugin::Scene::Resource::SceneManager::_unloadScene(ES::Engine::Registry &registry, const std::string &name)
{
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
        std::cerr << "[ERROR] ES::Plugin::Scene::Resource::SceneManager: Scene not found: " << name << std::endl;
        return std::nullopt;
    }
}
