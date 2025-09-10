#include <gtest/gtest.h>

#include "component/Vertex.hpp"

using namespace ES::Plugin::Object;

TEST(VertexTest, component_vertex_struct_size_comparaison)
{
    Component::Vertex vertex{};
    vertex.pos = {1.0f, 2.0f, 3.0f};
    vertex.normal = {4.0f, 5.0f, 6.0f};
    vertex.texCoord = {7.0f, 8.0f};

    EXPECT_EQ((sizeof(glm::vec3(1)) + sizeof(glm::vec3(1)) + sizeof(glm::vec2(1, 2))), sizeof(vertex));
}

TEST(VertexTest, component_vertex_equal_operator)
{
    Component::Vertex vertex1{};
    vertex1.pos = {1.0f, 2.0f, 3.0f};
    vertex1.normal = {4.0f, 5.0f, 6.0f};
    vertex1.texCoord = {7.0f, 8.0f};

    Component::Vertex vertex2{};
    vertex2.pos = {1.0f, 2.0f, 3.0f};
    vertex2.normal = {4.0f, 5.0f, 6.0f};
    vertex2.texCoord = {7.0f, 8.0f};

    EXPECT_EQ(vertex1 == vertex2, true);
}

TEST(VertexTest, component_vertex_equal_operator_fail)
{
    Component::Vertex vertex1{};
    vertex1.pos = {1.0f, 2.0f, 3.0f};
    vertex1.normal = {4.0f, 5.0f, 6.0f};
    vertex1.texCoord = {7.0f, 8.0f};

    Component::Vertex vertex2{};
    vertex2.pos = {1.0f, 150.0f, 3.0f};
    vertex2.normal = {4.0f, 5.0f, 6.0f};
    vertex2.texCoord = {7.0f, 9.0f};

    EXPECT_EQ(vertex1 == vertex2, false);
}

TEST(VertexTest, component_vertex_hash_extension)
{
    Component::Vertex vertex{};
    vertex.pos = {1.0f, 2.0f, 3.0f};
    vertex.normal = {4.0f, 5.0f, 6.0f};
    vertex.texCoord = {7.0f, 8.0f};

    std::unordered_map<Component::Vertex, uint32_t> vertices;
    vertices[vertex] = 0;

    EXPECT_EQ(vertices.size(), 1);
    EXPECT_EQ(vertices[vertex], 0);
}
