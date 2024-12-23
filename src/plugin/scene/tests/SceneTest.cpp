#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "Engine.hpp"

#include "String.hpp"

#include "Scene.hpp"

using namespace ES::Plugin::Scene;

class SceneTest : public Utils::AScene {
  protected:
    void _onCreate(ES::Engine::Registry &registry) final
    {
        // This method is intentionally left empty because no initialization is needed for this test scene.
    }

    void _onDestroy(ES::Engine::Registry &registry) final
    {
        // This method is intentionally left empty because no initialization is needed for this test scene.
    }
};

TEST(Scene, SceneManager)
{
    ES::Engine::Registry registry;
    registry.RegisterResource<Resource::SceneManager>(Resource::SceneManager());
    registry.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene1");
    registry.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene2");

    registry.GetResource<Resource::SceneManager>().SetNextScene("scene1");

    testing::internal::CaptureStdout();
    registry.RegisterSystem(System::UpdateScene);
    registry.RunSystems();

    registry.GetResource<Resource::SceneManager>().SetNextScene("scene2");
    registry.RunSystems();
    std::vector<std::string> output = ES::Plugin::Utils::String::Split(testing::internal::GetCapturedStdout(), '\n');
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[0], "Loading scene: scene1"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[1], "Unloading scene: scene1"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[2], "Loading scene: scene2"));
}
