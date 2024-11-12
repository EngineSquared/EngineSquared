#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <type_traits>

#include "Engine.hpp"

#include "AScene.hpp"

namespace ES::Plugin::Scene::Resource {
class SceneManager {
  public:
    SceneManager() = default;
    ~SceneManager() = default;

    /**
     * @brief Set the next scene to load.
     * It will be loaded at the next call of Update.
     *
     * @param name  name of the scene to load
     */
    inline void SetNextScene(const std::string &name) { _nextScene = name; }

    /**
     * @brief Unload the current scene and load the next scene.
     *
     * @param registry  registry that contains all components
     */
    void Update(ES::Engine::Registry &registry);

    /**
     * @brief Register a scene using a name as a key.
     *
     * @tparam TScene   type of the scene to register
     * @param name      name of the scene
     * @return          reference of the registered scene
     */
    template <typename TScene> TScene &RegisterScene(const std::string &name)
    {
        static_assert(std::is_base_of<ES::Plugin::Scene::Utils::AScene, TScene>::value,
                      "TScene must inherit from ES::Plugin::Scene::Utils::AScene");
        if (_scenes.find(name) != _scenes.end())
        {
            std::cerr << "[WARNING] ES::Plugin::Scene::Resource::SceneManager: Scene " << name << " already exists"
                      << std::endl;
        }
        std::shared_ptr<TScene> new_scene = std::make_shared<TScene>();
        _scenes[name] = new_scene;
        return *new_scene.get();
    }

  private:
    void _loadScene(ES::Engine::Registry &registry, const std::string &name);

    void _unloadScene(ES::Engine::Registry &registry, const std::string &name);

    [[nodiscard]] std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> _getScene(const std::string &name);

    std::map<std::string, std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> _scenes;
    std::optional<std::string> _nextScene;
    std::optional<std::string> _currentScene;
};
} // namespace ES::Plugin::Scene::Resource
