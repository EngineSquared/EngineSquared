#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"

void TestSystem(Engine::Core &core)
{
    auto entity = core.CreateEntity();
    auto meshComponent = entity.AddComponent<Object::Component::Mesh>(core);

    meshComponent.vertices = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
    meshComponent.normals = { {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f} };
    meshComponent.texCoords = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f} };

    auto pointBuffer = Graphic::Resource::PointGPUBuffer(entity);

    EXPECT_FALSE(pointBuffer.IsCreated(core));
    pointBuffer.Create(core);
    EXPECT_TRUE(pointBuffer.IsCreated(core));

    EXPECT_NO_THROW(pointBuffer.Update(core));

    EXPECT_EQ(pointBuffer.GetBuffer().getSize(), sizeof(float) * 24);

    pointBuffer.Destroy(core);
    EXPECT_FALSE(pointBuffer.IsCreated(core));
}

TEST(GraphicPlugin, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Graphic::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &c) {
        c.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(TestSystem);

    EXPECT_NO_THROW(core.RunSystems());
}
