#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "component/AmbientLight.hpp"
#include "component/Camera.hpp"
#include "component/DirectionalLight.hpp"
#include "component/GPUDirectionalLight.hpp"
#include "component/Mesh.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "plugin/PluginDefaultPipeline.hpp"
#include "resource/pass/Deferred.hpp"
#include "resource/pass/GBuffer.hpp"
#include "utils/ConfigureHeadlessGraphics.hpp"
#include "utils/ShapeGenerator.hpp"
#include "utils/ThrowErrorIfGraphicalErrorHappened.hpp"

void TestSystem(Engine::Core &core)
{
    auto cube = core.CreateEntity();

    cube.AddComponent<Object::Component::Transform>().SetRotation(
        glm::quat(glm::vec3(glm::radians(10.f), glm::radians(45.0f), 0.0f)));
    cube.AddComponent<Object::Component::Mesh>(Object::Utils::GenerateCubeMesh());

    auto camera = core.CreateEntity();

    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, -2.0f));
    camera.AddComponent<Object::Component::Camera>().fov = glm::radians(90.0f);

    auto ambientLight = core.CreateEntity();
    ambientLight.AddComponent<Object::Component::AmbientLight>().color = glm::vec3(0.2f);

    auto bluePointLight = core.CreateEntity();
    bluePointLight.AddComponent<Object::Component::Transform>(glm::vec3(3.0f, 0.0f, 0.0f));
    bluePointLight.AddComponent<Object::Component::PointLight>(
        Object::Component::PointLight{.color = glm::vec3(0.2f, 0.2f, 1.0f),
                                      .intensity = 50.0f,
                                      .radius = 10.0f,
                                      .falloff = 1.0f});

    auto directionalLight = core.CreateEntity();
    directionalLight.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 0.0f, -2.0f));
    directionalLight.AddComponent<Object::Component::DirectionalLight>({.color = glm::vec4(1.0f)});
}

void ExtractTextures(Engine::Core &core)
{
    auto &context = core.GetResource<Graphic::Resource::Context>();
    auto &textures = core.GetResource<Graphic::Resource::TextureContainer>();

    auto &normalTexture = textures.Get(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_NORMAL_ID);
    auto normalImage = normalTexture.RetrieveImage(context);

    auto &albedoTexture = textures.Get(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_ALBEDO_ID);
    auto albedoImage = albedoTexture.RetrieveImage(context);

    auto &depthTexture = textures.Get(DefaultPipeline::Resource::GBUFFER_PASS_OUTPUT_DEPTH_ID);
    auto depthImage = depthTexture.RetrieveImage(context);

    auto &outputTexture = textures.Get(Graphic::Utils::END_RENDER_TEXTURE_ID);
    auto outputTextureImage = outputTexture.RetrieveImage(context);

    auto view = core.GetRegistry().view<DefaultPipeline::Component::GPUDirectionalLight>();
    auto &shadowOneTexture = textures.Get(DefaultPipeline::Utils::DIRECTIONAL_LIGHTS_SHADOW_TEXTURE_ID);
    auto shadowOneTextureImage = shadowOneTexture.RetrieveImage(context);

    normalImage.ToPng("GBUFFER_NORMAL.png");
    albedoImage.ToPng("GBUFFER_ALBEDO.png");
    depthImage.ToPng("GBUFFER_DEPTH.png");
    outputTextureImage.ToPng("DEFERRED_OUTPUT.png");
    // shadowOneTextureImage.ToPng("SHADOW_ONE_OUTPUT.png");
}

TEST(DefaultPipeline, SmokeTest)
{
    Engine::Core core;

    core.AddPlugins<DefaultPipeline::Plugin>();

    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);

    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);

    core.RegisterSystem(TestSystem);

    // Uncomment to save the images
    core.RegisterSystem<RenderingPipeline::CommandCreation>(ExtractTextures);

    core.RunSystems();

    SUCCEED();
}
