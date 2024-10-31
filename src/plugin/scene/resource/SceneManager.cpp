#include <iostream>
#include <optional>
#include <string>

#include "Engine.hpp"

#include "AScene.hpp"

#include "SceneManager.hpp"

void ES::Plugin::Scene::Resource::SceneManager::Update(ES::Engine::Registry &registry)
{
    if (nextScene.has_value())
    {
        if (currentScene.has_value())
        {
            UnloadScene(registry, currentScene.value());
        }
        LoadScene(registry, nextScene.value());
        currentScene = nextScene;
        nextScene.reset();
    }
}
void ES::Plugin::Scene::Resource::SceneManager::LoadScene(ES::Engine::Registry &registry, const std::string &name)
{
    auto scene = scenes.find(name);
    if (scene != scenes.end())
    {
        scene->second->Load(registry);
    }
    else
    {
        std::cerr << "[ERROR] ES::Plugin::Scene::Resource::SceneManager: Scene " << name << " not found" << std::endl;
    }
}

void ES::Plugin::Scene::Resource::SceneManager::UnloadScene(ES::Engine::Registry &registry, const std::string &name)
{
    auto scene = scenes.find(name);
    if (scene != scenes.end())
    {
        scene->second->Unload(registry);
    }
    else
    {
        std::cerr << "[ERROR] ES::Plugin::Scene::Resource::SceneManager: Scene " << name << " not found" << std::endl;
    }
}
