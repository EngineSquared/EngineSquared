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
    });
}

TEST(OBJLoaderTest, load_empty_path)
{
    EXPECT_THROW(OBJLoader(""), std::exception);
}

TEST(OBJLoaderTest, load_not_obj_file)
{
    EXPECT_THROW(OBJLoader(OBJ_FILE_PATH "not_obj.txt"), std::exception);
}

TEST(OBJLoaderTest, load_wrong_path)
{
    EXPECT_THROW(OBJLoader("wrong_path"), std::exception);
}
