#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "Engine.hpp"

#include "String.hpp"

#include "Scene.hpp"

using namespace ES::Plugin::Scene;

class SceneTest : public Utils::AScene {
  protected:
    void _onCreate(ES::Engine::Core &registry) final
    {
        // This method is intentionally left empty because no initialization is needed for this test scene.
    }

    void _onDestroy(ES::Engine::Core &registry) final
    {
        // This method is intentionally left empty because no initialization is needed for this test scene.
    }
};

TEST(Scene, SceneManager)
{
    ES::Engine::Core core;
    core.RegisterResource<Resource::SceneManager>(Resource::SceneManager());
    core.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene1");
    core.GetResource<Resource::SceneManager>().RegisterScene<SceneTest>("scene2");

    core.GetResource<Resource::SceneManager>().SetNextScene("scene1");

    testing::internal::CaptureStdout();
    core.RegisterSystem(System::UpdateScene);
    core.RunSystems();

    core.GetResource<Resource::SceneManager>().SetNextScene("scene2");
    core.RunSystems();
    std::vector<std::string> output = ES::Plugin::Utils::String::Split(testing::internal::GetCapturedStdout(), '\n');
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[0], "Loading scene: scene1"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[1], "Unloading scene: scene1"));
    EXPECT_TRUE(ES::Plugin::Utils::String::EndsWith(output[2], "Loading scene: scene2"));
}
