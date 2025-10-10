#include <gtest/gtest.h>

#include "Graphic.hpp"

TEST(GraphicPlugin, GlobalRun)
{
    Plugin::Graphic::Utils::ShaderBuilder builder;

    builder.setShader("void main() { }")
        .addVertexAttribute(wgpu::VertexFormat::Float32x3, 0, 0)
        .addVertexAttribute(wgpu::VertexFormat::Float32x2, 3 * sizeof(float), 1);

    EXPECT_THROW(builder.addVertexAttribute(wgpu::VertexFormat::Float32x3, 5 * sizeof(float), 0),
                 Plugin::Graphic::Exception::DuplicatedVertexAttributeLocationError);
    EXPECT_THROW(builder.addVertexAttribute(wgpu::VertexFormat::Float32x3, 2 * sizeof(float), 2),
                 Plugin::Graphic::Exception::VertexAttributeOverlappingError);
}
