#include <gtest/gtest.h>

#include "component/Mesh.hpp"

using namespace Object;

TEST(Mesh, struct_size_comparaison)
{
    Component::Mesh mesh{};
    mesh.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh.EmplaceNormals(glm::vec3(4.0f, 5.0f, 6.0f));
    mesh.EmplaceTexCoords(glm::vec2(7.0f, 8.0f));
    mesh.EmplaceIndices(9);

    EXPECT_EQ(mesh.GetVertices().size(), 1u);
    EXPECT_EQ(mesh.GetNormals().size(), 1u);
    EXPECT_EQ(mesh.GetTexCoords().size(), 1u);
    EXPECT_EQ(mesh.GetIndices().size(), 1u);
}

TEST(Mesh, set_and_get_vertex)
{
    Component::Mesh mesh{};
    mesh.EmplaceVertices(glm::vec3(0.0f, 0.0f, 0.0f));
    mesh.SetVertexAt(0, glm::vec3(1.0f, 2.0f, 3.0f));

    const auto &vertex = mesh.GetVertices().at(0);
    EXPECT_EQ(vertex, glm::vec3(1.0f, 2.0f, 3.0f));
}

TEST(Mesh, set_and_get_normal)
{
    Component::Mesh mesh{};
    mesh.EmplaceNormals(glm::vec3(0.0f, 0.0f, 0.0f));
    mesh.SetNormalAt(0, glm::vec3(4.0f, 5.0f, 6.0f));

    const auto &normal = mesh.GetNormals().at(0);
    EXPECT_EQ(normal, glm::vec3(4.0f, 5.0f, 6.0f));
}

TEST(Mesh, set_and_get_texcoord)
{
    Component::Mesh mesh{};
    mesh.EmplaceTexCoords(glm::vec2(0.0f, 0.0f));
    mesh.SetTexCoordAt(0, glm::vec2(7.0f, 8.0f));

    const auto &texCoord = mesh.GetTexCoords().at(0);
    EXPECT_EQ(texCoord, glm::vec2(7.0f, 8.0f));
}

TEST(Mesh, reserve_and_emplace_vertices)
{
    Component::Mesh mesh{};
    mesh.ReserveVertices(2u);
    mesh.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh.EmplaceVertices(glm::vec3(4.0f, 5.0f, 6.0f));

    EXPECT_EQ(mesh.GetVertices().size(), 2u);
    EXPECT_EQ(mesh.GetVertices().at(0), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(mesh.GetVertices().at(1), glm::vec3(4.0f, 5.0f, 6.0f));
}

TEST(Mesh, reserve_and_emplace_normals)
{
    Component::Mesh mesh{};
    mesh.ReserveNormals(2u);
    mesh.EmplaceNormals(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh.EmplaceNormals(glm::vec3(4.0f, 5.0f, 6.0f));

    EXPECT_EQ(mesh.GetNormals().size(), 2u);
    EXPECT_EQ(mesh.GetNormals().at(0), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(mesh.GetNormals().at(1), glm::vec3(4.0f, 5.0f, 6.0f));
}

TEST(Mesh, reserve_and_emplace_texcoords)
{
    Component::Mesh mesh{};
    mesh.ReserveTexCoords(2u);
    mesh.EmplaceTexCoords(glm::vec2(1.0f, 2.0f));
    mesh.EmplaceTexCoords(glm::vec2(3.0f, 4.0f));

    EXPECT_EQ(mesh.GetTexCoords().size(), 2u);
    EXPECT_EQ(mesh.GetTexCoords().at(0), glm::vec2(1.0f, 2.0f));
    EXPECT_EQ(mesh.GetTexCoords().at(1), glm::vec2(3.0f, 4.0f));
}

TEST(Mesh, copy_constructor)
{
    Component::Mesh mesh1{};
    mesh1.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh1.EmplaceNormals(glm::vec3(4.0f, 5.0f, 6.0f));
    mesh1.EmplaceTexCoords(glm::vec2(7.0f, 8.0f));
    mesh1.EmplaceIndices(9);

    Component::Mesh mesh2 = mesh1; // Copy constructor

    EXPECT_EQ(mesh2.GetVertices().size(), 1u);
    EXPECT_EQ(mesh2.GetVertices().at(0), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(mesh2.GetNormals().size(), 1u);
    EXPECT_EQ(mesh2.GetNormals().at(0), glm::vec3(4.0f, 5.0f, 6.0f));
    EXPECT_EQ(mesh2.GetTexCoords().size(), 1u);
    EXPECT_EQ(mesh2.GetTexCoords().at(0), glm::vec2(7.0f, 8.0f));
    EXPECT_EQ(mesh2.GetIndices().size(), 1u);
    EXPECT_EQ(mesh2.GetIndices().at(0), 9u);
}

TEST(Mesh, copy_assignment_operator)
{
    Component::Mesh mesh1{};
    mesh1.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh1.EmplaceNormals(glm::vec3(4.0f, 5.0f, 6.0f));
    mesh1.EmplaceTexCoords(glm::vec2(7.0f, 8.0f));
    mesh1.EmplaceIndices(9);

    Component::Mesh mesh2{};
    mesh2 = mesh1; // Copy assignment operator

    EXPECT_EQ(mesh2.GetVertices().size(), 1u);
    EXPECT_EQ(mesh2.GetVertices().at(0), glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_EQ(mesh2.GetNormals().size(), 1u);
    EXPECT_EQ(mesh2.GetNormals().at(0), glm::vec3(4.0f, 5.0f, 6.0f));
    EXPECT_EQ(mesh2.GetTexCoords().size(), 1u);
    EXPECT_EQ(mesh2.GetTexCoords().at(0), glm::vec2(7.0f, 8.0f));
    EXPECT_EQ(mesh2.GetIndices().size(), 1u);
    EXPECT_EQ(mesh2.GetIndices().at(0), 9u);
}

TEST(Mesh, dirty_flag_on_setters)
{
    Component::Mesh mesh{};
    EXPECT_FALSE(mesh.IsDirty());
    mesh.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_TRUE(mesh.IsDirty());
    mesh.ClearDirty();
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetVertexAt(0, glm::vec3(4.0f, 5.0f, 6.0f));
    EXPECT_TRUE(mesh.IsDirty());
}

TEST(Mesh, dirty_flag_on_emplace)
{
    Component::Mesh mesh{};
    EXPECT_FALSE(mesh.IsDirty());
    mesh.EmplaceNormals(glm::vec3(1.0f, 2.0f, 3.0f));
    EXPECT_TRUE(mesh.IsDirty());
    mesh.ClearDirty();
    EXPECT_FALSE(mesh.IsDirty());
    mesh.EmplaceTexCoords(glm::vec2(4.0f, 5.0f));
    EXPECT_TRUE(mesh.IsDirty());
}

TEST(Mesh, dirty_flag_on_setters_multiple)
{
    Component::Mesh mesh{};
    EXPECT_FALSE(mesh.IsDirty());
    mesh.EmplaceVertices(glm::vec3(1.0f, 2.0f, 3.0f));
    mesh.EmplaceNormals(glm::vec3(4.0f, 5.0f, 6.0f));
    mesh.EmplaceTexCoords(glm::vec2(0.0f, 0.0f));
    EXPECT_TRUE(mesh.IsDirty());
    mesh.ClearDirty();
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetTexCoordAt(0, glm::vec2(7.0f, 8.0f));
    EXPECT_TRUE(mesh.IsDirty());
}

TEST(Mesh, dirty_flag_on_setters_full)
{
    Component::Mesh mesh{};
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetVertices({glm::vec3(1.0f, 2.0f, 3.0f)});
    EXPECT_TRUE(mesh.IsDirty());
    mesh.ClearDirty();
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetNormals({glm::vec3(4.0f, 5.0f, 6.0f)});
    EXPECT_TRUE(mesh.IsDirty());
}

TEST(Mesh, dirty_flag_on_setters_texcoords)
{
    Component::Mesh mesh{};
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetTexCoords({glm::vec2(7.0f, 8.0f)});
    EXPECT_TRUE(mesh.IsDirty());
    mesh.ClearDirty();
    EXPECT_FALSE(mesh.IsDirty());
    mesh.SetIndices({9});
    EXPECT_TRUE(mesh.IsDirty());
}
