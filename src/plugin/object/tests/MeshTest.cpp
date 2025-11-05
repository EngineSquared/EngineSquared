#include <gtest/gtest.h>

#include "component/Mesh.hpp"

using namespace Object;

TEST(Mesh, struct_size_comparaison)
{
    Component::Mesh mesh{};
    mesh.vertices.emplace_back(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh.normals.emplace_back(glm::vec3(4.0f, 5.0f, 6.0f));
    mesh.texCoords.emplace_back(glm::vec2(7.0f, 8.0f));
    mesh.indices.emplace_back(9);

    EXPECT_EQ(mesh.vertices.size(), 1u);
    EXPECT_EQ(mesh.normals.size(), 1u);
    EXPECT_EQ(mesh.texCoords.size(), 1u);
    EXPECT_EQ(mesh.indices.size(), 1u);
}
