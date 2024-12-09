#include <gtest/gtest.h>

#include "OBJLoader.hpp"

using namespace ES::Plugin::Object;

TEST(OBJLoader, loadModel)
{
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("assets/cube.obj", vertices, indices), true);
}

TEST(OBJLoader, loadModel_fail)
{
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("assets/cube_fail.obj", vertices, indices), false);
}

TEST(OBJLoader, loadModel_empty_path)
{
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("", vertices, indices), false);
}

TEST(OBJLoader, loadModel_not_obj_file)
{
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("assets/cube.png", vertices, indices), false);
}

TEST(OBJLoader, loadModel_wrong_path)
{
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    EXPECT_EQ(Resource::OBJLoader::loadModel("wrong_path", vertices, indices), false);
}
