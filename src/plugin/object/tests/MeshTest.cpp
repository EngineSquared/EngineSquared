#include <gtest/gtest.h>

#include "component/Mesh.hpp"

#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>

#include "export.h"

#define OBJ_FILE_PATH PROJECT_SOURCE_DIR "assets/"

using namespace ES::Plugin::Object;

/**
 * @brief Load a mesh from a file using the entt resource cache.
 *        The mesh is loaded from a file and stored in the cache.
 *        The mesh is then retrieved from the cache and its vertices are checked.
 */
TEST(MeshTest, component_mesh_entt_cache_loader)
{
    entt::resource_cache<Component::Mesh, Component::MeshLoader> cache{};

    std::string something_else("mesh_id");
    auto ret = cache.load(entt::hashed_string{something_else.c_str()}, std::string(OBJ_FILE_PATH "cube.obj"));

    EXPECT_EQ(ret.second, true);

    entt::resource<Component::Mesh> res = ret.first->second;

    EXPECT_EQ(res->vertices.empty(), false);
    EXPECT_EQ(res->indices.empty(), false);
}
