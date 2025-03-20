#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "Engine.hpp"

#include "String.hpp"

#include "Scene.hpp"

using namespace ES::Plugin::Scene;

class SceneTest : public Utils::AScene {
    inline static int _numScenes = 0;

  public:
    SceneTest() : Utils::AScene() {}

  protected:
    void _onCreate(ES::Engine::Core &core) final
    {
        core.GetResource<std::vector<std::string>>().push_back(fmt::format("Creating scene: {}", _sceneId));
    }

    void _onDestroy(ES::Engine::Core &core) final
    {
        core.GetResource<std::vector<std::string>>().push_back(fmt::format("Destroying scene: {}", _sceneId));
    }

  private:
    int _sceneId = _numScenes++;
};

TEST(Scene, SceneManager)
{
    ES::Engine::Core core;
    core.RegisterResource<std::vector<std::string>>(std::vector<std::string>());
    core.RegisterResource<Resource::SceneManager>(Resource::SceneManager());
    core.RegisterSystem(System::UpdateScene);

    core.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene1");
    core.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene2");

    core.GetResource<Resource::SceneManager>().SetNextScene("scene1");
    core.RunSystems();

    core.GetResource<Resource::SceneManager>().SetNextScene("scene2");
    core.RunSystems();

    std::vector<std::string> output = core.GetResource<std::vector<std::string>>();

    EXPECT_EQ(output.size(), 3);
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[0], "Creating scene: 0"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[1], "Destroying scene: 0"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[2], "Creating scene: 1"));
}
