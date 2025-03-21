#include <gtest/gtest.h>

#include "OBJLoader.hpp"

#include "export.h"

#define OBJ_FILE_PATH PROJECT_SOURCE_DIR "assets/"

using namespace ES::Plugin::Object;

TEST(OBJLoaderTest, loadModel)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel(OBJ_FILE_PATH "cube.obj", vertices, normals, texCoords, indices), true);

    EXPECT_EQ(vertices.empty(), false);
    EXPECT_EQ(indices.empty(), false);
}

TEST(OBJLoaderTest, loadModel_empty_path)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("", vertices, normals, texCoords, indices), false);
}

TEST(OBJLoaderTest, loadModel_not_obj_file)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel(OBJ_FILE_PATH "not_obj.txt", vertices, normals, texCoords, indices), false);
}

TEST(OBJLoaderTest, loadModel_wrong_path)
{
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("wrong_path", vertices, normals, texCoords, indices), false);
}
