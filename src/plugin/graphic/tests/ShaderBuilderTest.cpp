#include <gtest/gtest.h>

#include "Graphic.hpp"

TEST(GraphicPlugin, GlobalRun)
{
    Plugin::Graphic::Utils::ShaderBuilder builder;

    builder.setShader("void main() { }")
        .addVertexBufferLayout(Plugin::Graphic::Utils::VertexBufferLayout()
                                   .addVertexAttribute(wgpu::VertexFormat::Float32x2, 3 * sizeof(float), 1)
                                   .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
                                   .setArrayStride(5 * sizeof(float))
                                   .setStepMode(wgpu::VertexStepMode::Vertex) // This is done by default
        );

    EXPECT_THROW(
        builder.getVertexBufferLayout(0).addVertexAttribute(wgpu::VertexFormat::Float32x3, 5 * sizeof(float), 0),
        Plugin::Graphic::Exception::DuplicatedVertexAttributeLocationError);
    EXPECT_THROW(
        builder.getVertexBufferLayout(0).addVertexAttribute(wgpu::VertexFormat::Float32x3, 2 * sizeof(float), 2),
        Plugin::Graphic::Exception::VertexAttributeOverlappingError);
    EXPECT_THROW(builder.getVertexBufferLayout(0).addVertexAttribute(wgpu::VertexFormat::Force32, 2 * sizeof(float), 2),
                 Plugin::Graphic::Exception::UnknownFormatType);
}
