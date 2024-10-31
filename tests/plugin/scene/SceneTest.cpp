#include <iostream>
#include <string>

#include <gtest/gtest.h>

#include "Engine.hpp"

#include "Scene.hpp"

using namespace ES::Plugin::Scene;

class SceneTest1 : public Utils::AScene {
  protected:
    virtual void OnCreate(ES::Engine::Registry &registry) { std::cout << "Scene 1 Created" << std::endl; }

    virtual void OnDestroy(ES::Engine::Registry &registry) { std::cout << "Scene 1 Destroyed" << std::endl; }
};

class SceneTest2 : public Utils::AScene {
  protected:
    virtual void OnCreate(ES::Engine::Registry &registry) { std::cout << "Scene 2 Created" << std::endl; }

    virtual void OnDestroy(ES::Engine::Registry &registry) { std::cout << "Scene 2 Destroyed" << std::endl; }
};

TEST(Scene, SceneManager)
{
    ES::Engine::Registry registry;
    registry.RegisterResource<ES::Plugin::Scene::Resource::SceneManager>(ES::Plugin::Scene::Resource::SceneManager());
    registry.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<SceneTest1>("scene1");
    registry.GetResource<ES::Plugin::Scene::Resource::SceneManager>().RegisterScene<SceneTest2>("scene2");

    registry.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("scene1");

    testing::internal::CaptureStdout();
    registry.RegisterSystem(ES::Plugin::Scene::System::UpdateScene);
    registry.RunSystems();

    registry.GetResource<ES::Plugin::Scene::Resource::SceneManager>().SetNextScene("scene2");
    registry.RunSystems();
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "Scene 1 Created\nScene 1 Destroyed\nScene 2 Created\n");
}
