#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <type_traits>

#include "Engine.hpp"

#include "Logger.hpp"

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
    inline void SetNextScene(const std::string_view &name) { _nextScene = name; }

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
        static_assert(std::is_base_of_v<ES::Plugin::Scene::Utils::AScene, TScene>,
                      "TScene must inherit from ES::Plugin::Scene::Utils::AScene");
        if (_scenes.contains(name))
        {
            ES::Utils::Log::Warn("Scene " + name + " already exists");
        }
        std::shared_ptr<TScene> new_scene = std::make_shared<TScene>();
        _scenes[name] = new_scene;
        return *new_scene.get();
    }

  private:
    void _loadScene(ES::Engine::Registry &registry, const std::string &name);

    void _unloadScene(ES::Engine::Registry &registry, const std::string &name);

    [[nodiscard]] std::optional<std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> _getScene(const std::string &name);

    struct TransparentHash {
        using is_transparent = void;
        std::size_t operator()(const std::string &str) const noexcept { return std::hash<std::string>{}(str); }
        std::size_t operator()(std::string_view str) const noexcept { return std::hash<std::string_view>{}(str); }
        std::size_t operator()(const char *str) const noexcept { return std::hash<std::string_view>{}(str); }
    };
    std::unordered_map<std::string, std::shared_ptr<ES::Plugin::Scene::Utils::AScene>, TransparentHash, std::equal_to<>>
        _scenes;

    std::optional<std::string> _nextScene;
    std::optional<std::string> _currentScene;
};
} // namespace ES::Plugin::Scene::Resource
