#include <algorithm>
#include <gtest/gtest.h>

#include "resource/OBJLoader.hpp"

#include "export.h"

#define OBJ_FILE_PATH PROJECT_SOURCE_DIR "assets/"

using namespace Object;

TEST(OBJLoaderTest, load_obj_file)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube.obj");
        Component::Mesh mesh = loader.GetMesh();

        EXPECT_FALSE(mesh.GetVertices().empty());
        EXPECT_FALSE(mesh.GetIndices().empty());
        EXPECT_FALSE(mesh.GetNormals().empty());
        EXPECT_FALSE(mesh.GetTexCoords().empty());

        Component::Mesh mesh2 = loader.GetMesh();
        EXPECT_EQ(mesh.GetVertices(), mesh2.GetVertices());
        EXPECT_EQ(mesh.GetIndices(), mesh2.GetIndices());
        EXPECT_EQ(mesh.GetNormals(), mesh2.GetNormals());
        EXPECT_EQ(mesh.GetTexCoords(), mesh2.GetTexCoords());
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

        for (auto [mesh, material, name] : shapes)
        {
            EXPECT_FALSE(mesh.GetVertices().empty());
            EXPECT_FALSE(mesh.GetIndices().empty());
            EXPECT_FALSE(mesh.GetNormals().empty());
            EXPECT_FALSE(mesh.GetTexCoords().empty());
            EXPECT_TRUE(material.name.empty());
        }

        for (const auto &shape : shapes)
        {
            const auto &mesh = shape.GetMesh();
            EXPECT_FALSE(mesh.GetVertices().empty());
            EXPECT_FALSE(mesh.GetIndices().empty());
            EXPECT_FALSE(mesh.GetNormals().empty());
            EXPECT_FALSE(mesh.GetTexCoords().empty());
            EXPECT_TRUE(shape.GetMaterial().name.empty());
        }

        auto shapes2 = loader.GetShapes();
        EXPECT_EQ(shapes.size(), shapes2.size());
        for (size_t i = 0; i < shapes.size(); ++i)
        {
            EXPECT_EQ(shapes[i].mesh.GetVertices(), shapes2[i].mesh.GetVertices());
            EXPECT_EQ(shapes[i].mesh.GetIndices(), shapes2[i].mesh.GetIndices());
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

TEST(OBJLoaderTest, get_shapes_materials_consistent)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube_with_mat.obj");

        auto shapes = loader.GetShapes();
        EXPECT_FALSE(shapes.empty());

        auto materials = loader.GetMaterials();
        EXPECT_FALSE(materials.empty());

        for (const auto &[mesh, material, name] : shapes)
        {
            EXPECT_FALSE(mesh.GetVertices().empty());
            EXPECT_FALSE(mesh.GetIndices().empty());
            EXPECT_FALSE(mesh.GetNormals().empty());
            EXPECT_FALSE(mesh.GetTexCoords().empty());
            EXPECT_FALSE(material.name.empty());

            const auto it = std::find_if(materials.begin(), materials.end(),
                                         [&material](const Component::Material &m) { return m.name == material.name; });
            EXPECT_NE(it, materials.end());
            if (it != materials.end())
            {
                EXPECT_EQ(it->ambient, material.ambient);
                EXPECT_EQ(it->diffuse, material.diffuse);
                EXPECT_EQ(it->specular, material.specular);
                EXPECT_EQ(it->shininess, material.shininess);
                EXPECT_EQ(it->ior, material.ior);
                EXPECT_EQ(it->dissolve, material.dissolve);
            }
        }
    });
}
