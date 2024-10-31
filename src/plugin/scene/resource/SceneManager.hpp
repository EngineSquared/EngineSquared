#pragma once

#include <string>
#include <iostream>
#include <optional>
#include <type_traits>

#include "Engine.hpp"

#include "AScene.hpp"

namespace ES::Plugin::Scene::Resource {
    class SceneManager {
    public:
        SceneManager() = default;
        ~SceneManager() = default;

        inline void SetNextScene(const std::string &name) { nextScene = name; }

        void Update(ES::Engine::Registry &registry);

        template <typename TScene>
        TScene &RegisterScene(const std::string &name) 
        {
            if constexpr (!std::is_base_of<ES::Plugin::Scene::Utils::AScene, TScene>::value) {    
                static_assert(false, "TScene must inherit from ES::Plugin::Scene::Utils::AScene");
            } else {
                if (scenes.find(name) != scenes.end()) {
                    std::cerr << "[WARNING] ES::Plugin::Scene::Resource::SceneManager: Scene " << name << " already exists" << std::endl;
                }
                std::shared_ptr<TScene> new_scene = std::make_shared<TScene>();
                scenes[name] = new_scene;
                return *new_scene.get();
            }
        }

    private:
        void LoadScene(ES::Engine::Registry &registry, const std::string &name);

        void UnloadScene(ES::Engine::Registry &registry, const std::string &name);

        std::map<std::string, std::shared_ptr<ES::Plugin::Scene::Utils::AScene>> scenes;
        std::optional<std::string> nextScene;
        std::optional<std::string> currentScene;
    };
}