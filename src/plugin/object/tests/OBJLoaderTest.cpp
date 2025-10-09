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

TEST(OBJLoaderTest, get_materials_loaded_from_mtl)
{
    ASSERT_NO_THROW({
        OBJLoader loader(OBJ_FILE_PATH "cube_with_mat.obj");

        auto materials = loader.GetMaterials();
        EXPECT_FALSE(materials.empty());

        const auto it = std::find_if(materials.begin(), materials.end(), [](const Component::Material &m) { return m.name == "newmat"; });
        EXPECT_NE(it, materials.end());

        if (it != materials.end())
        {
            const auto &mat = *it;
            constexpr double eps = 1e-6;

            EXPECT_NEAR(mat.ambient.x, 0.2, eps);
            EXPECT_NEAR(mat.ambient.y, 0.2, eps);
            EXPECT_NEAR(mat.ambient.z, 0.2, eps);

            EXPECT_NEAR(mat.diffuse.x, 0.8, eps);
            EXPECT_NEAR(mat.diffuse.y, 0.1, eps);
            EXPECT_NEAR(mat.diffuse.z, 0.1, eps);

            EXPECT_NEAR(mat.specular.x, 0.5, eps);
            EXPECT_NEAR(mat.specular.y, 0.5, eps);
            EXPECT_NEAR(mat.specular.z, 0.5, eps);

            EXPECT_NEAR(mat.shininess, 25.0, eps);
            EXPECT_NEAR(mat.ior, 1.45, eps);
            EXPECT_NEAR(mat.dissolve, 1.0, eps);
        }
    });
}
