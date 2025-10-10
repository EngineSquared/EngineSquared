#include <gtest/gtest.h>

#include "resource/OBJLoader.hpp"

#include "export.h"

#define OBJ_FILE_PATH PROJECT_SOURCE_DIR "assets/"

using namespace Plugin::Object;

TEST(OBJLoaderTest, load_obj_file)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube.obj");
        Component::Mesh mesh = loader.GetMesh();

        EXPECT_FALSE(mesh.vertices.empty());
        EXPECT_FALSE(mesh.indices.empty());
        EXPECT_FALSE(mesh.normals.empty());
        EXPECT_FALSE(mesh.texCoords.empty());

        Component::Mesh mesh2 = loader.GetMesh();
        EXPECT_EQ(mesh.vertices, mesh2.vertices);
        EXPECT_EQ(mesh.indices, mesh2.indices);
        EXPECT_EQ(mesh.normals, mesh2.normals);
        EXPECT_EQ(mesh.texCoords, mesh2.texCoords);
    });
}

TEST(OBJLoaderTest, load_empty_path) { EXPECT_THROW(OBJLoader(""), std::exception); }

TEST(OBJLoaderTest, load_not_obj_file) { EXPECT_THROW(OBJLoader(OBJ_FILE_PATH "not_obj.txt"), std::exception); }

TEST(OBJLoaderTest, load_wrong_path) { EXPECT_THROW(OBJLoader("wrong_path"), std::exception); }

TEST(OBJLoaderTest, get_shapes_iterable_and_consistent)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube.obj");

        auto shapes = loader.GetShapes();
        EXPECT_FALSE(shapes.empty());

        for (const auto &shape : shapes)
        {
            const auto &mesh = shape.GetMesh();
            EXPECT_FALSE(mesh.vertices.empty());
        }

        auto shapes2 = loader.GetShapes();
        EXPECT_EQ(shapes.size(), shapes2.size());
        for (size_t i = 0; i < shapes.size(); ++i)
        {
            EXPECT_EQ(shapes[i].mesh.vertices, shapes2[i].mesh.vertices);
            EXPECT_EQ(shapes[i].mesh.indices, shapes2[i].mesh.indices);
        }
    });
}

TEST(OBJLoaderTest, get_materials_loaded_from_mtl)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube_with_mat.obj");

        auto materials = loader.GetMaterials();
        EXPECT_FALSE(materials.empty());

        const auto it = std::find_if(materials.begin(), materials.end(),
                                     [](const Component::Material &m) { return m.name == "newmat"; });
        EXPECT_NE(it, materials.end());

        if (it != materials.end())
        {
            const auto &mat = *it;

            ASSERT_FLOAT_EQ(mat.ambient.x, 0.2f);
            ASSERT_FLOAT_EQ(mat.ambient.y, 0.2f);
            ASSERT_FLOAT_EQ(mat.ambient.z, 0.2f);

            ASSERT_FLOAT_EQ(mat.diffuse.x, 0.8f);
            ASSERT_FLOAT_EQ(mat.diffuse.y, 0.1f);
            ASSERT_FLOAT_EQ(mat.diffuse.z, 0.1f);

            ASSERT_FLOAT_EQ(mat.specular.x, 0.5f);
            ASSERT_FLOAT_EQ(mat.specular.y, 0.5f);
            ASSERT_FLOAT_EQ(mat.specular.z, 0.5f);

            ASSERT_FLOAT_EQ(mat.shininess, 25.0f);
            ASSERT_FLOAT_EQ(mat.ior, 1.45f);
            ASSERT_FLOAT_EQ(mat.dissolve, 1.0f);
        }
    });
}
