#include "CreateSkyBox.hpp"

#include "OpenGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <filesystem>
#include <stdexcept>

/**
 * @brief Creates a cube mesh for skybox rendering
 *
 * Generates a simple cube mesh with 36 vertices (6 faces × 6 vertices per face as triangles).
 * No optimization is performed - each triangle is defined independently.
 *
 * @param size Cube dimensions (width, height, depth) - must be positive values
 * @return ES::Plugin::Object::Component::Mesh Cube mesh with 36 vertices and 36 indices
 *
 * @note Vertices are defined as triangles without shared vertices
 * @note No normals or UV coordinates are generated
 * @note Face order: Back, Left, Right, Front, Top, Bottom
 * @note Performance: O(1) with fixed 36 vertices
 *
 * @code
 * // Internal usage for mesh generation
 * auto mesh = CreateSkyBoxMesh(glm::vec3(10.0f, 10.0f, 10.0f));
 * // mesh.vertices.size() == 36
 * // mesh.indices.size() == 36
 * @endcode
 */
static ES::Plugin::Object::Component::Mesh CreateSkyBoxMesh(const glm::vec3 &size) noexcept
{
    ES::Plugin::Object::Component::Mesh skybox_mesh;

    const std::array<glm::vec3, 36> skyboxVertices = {
        {// Back face (Z negative)
         {-size.x, size.y, -size.z},
         {-size.x, -size.y, -size.z},
         {size.x, -size.y, -size.z},
         {size.x, -size.y, -size.z},
         {size.x, size.y, -size.z},
         {-size.x, size.y, -size.z},

         // Left face (X negative)
         {-size.x, -size.y, size.z},
         {-size.x, -size.y, -size.z},
         {-size.x, size.y, -size.z},
         {-size.x, size.y, -size.z},
         {-size.x, size.y, size.z},
         {-size.x, -size.y, size.z},

         // Right face (X positive)
         {size.x, -size.y, -size.z},
         {size.x, -size.y, size.z},
         {size.x, size.y, size.z},
         {size.x, size.y, size.z},
         {size.x, size.y, -size.z},
         {size.x, -size.y, -size.z},

         // Front face (Z positive)
         {-size.x, -size.y, size.z},
         {-size.x, size.y, size.z},
         {size.x, size.y, size.z},
         {size.x, size.y, size.z},
         {size.x, -size.y, size.z},
         {-size.x, -size.y, size.z},

         // Top face (Y positive)
         {-size.x, size.y, -size.z},
         {size.x, size.y, -size.z},
         {size.x, size.y, size.z},
         {size.x, size.y, size.z},
         {-size.x, size.y, size.z},
         {-size.x, size.y, -size.z},

         // Bottom face (Y negative)
         {-size.x, -size.y, -size.z},
         {-size.x, -size.y, size.z},
         {size.x, -size.y, -size.z},
         {size.x, -size.y, -size.z},
         {-size.x, -size.y, size.z},
         {size.x, -size.y, size.z}}
    };

    skybox_mesh.vertices.reserve(36);
    skybox_mesh.indices.reserve(36);

    for (const auto &vertex : skyboxVertices)
        skybox_mesh.vertices.emplace_back(vertex);

    for (uint32_t i = 0; i < 36; ++i)
        skybox_mesh.indices.emplace_back(i);

    return skybox_mesh;
}

/**
 * @brief Creates the base skybox entity with transform and mesh components
 *
 * Creates a basic skybox entity with only Transform and Mesh components.
 * Input validation ensures size components are positive.
 *
 * @param core Reference to the engine core
 * @param position World position for the skybox
 * @param rotation World rotation for the skybox
 * @param size Physical dimensions of the skybox (must be positive)
 * @return ES::Engine::Entity Base skybox entity with Transform and Mesh components only
 *
 * @throws std::invalid_argument If size contains non-positive values
 * @note Scale is fixed to (1.0f, 1.0f, 1.0f) - size affects mesh generation, not transform scale
 * @note Entity contains no rendering components at this stage
 */
static ES::Engine::Entity CreateSkyBoxEntity(ES::Engine::Core &core, const glm::vec3 &position,
                                             const glm::quat &rotation, const glm::vec3 &size)
{
    if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f)
        throw std::invalid_argument("Skybox size components must be positive values");

    constexpr glm::vec3 box_scale{1.0f};

    auto box = core.CreateEntity();

    box.AddComponent<ES::Plugin::Object::Component::Transform>(core, position, box_scale, rotation);
    box.AddComponent<ES::Plugin::Object::Component::Mesh>(core, CreateSkyBoxMesh(size));

    return box;
}

/**
 * @brief Adds rendering components required for skybox rendering
 *
 * Adds MaterialHandle and ShaderHandle components with "skyboxDefault" identifiers.
 * These components integrate the skybox with the rendering pipeline.
 *
 * @param core Reference to the engine core
 * @param skybox The skybox entity to configure
 *
 * @note Adds MaterialHandle with "skyboxDefault" material
 * @note Adds ShaderHandle with "skyboxDefault" shader
 * @note CRITICAL: Skybox rendering requires specific depth function handling in render pipeline
 */
static void AddSkyboxRenderingComponents(ES::Engine::Core &core, ES::Engine::Entity &skybox)
{
    skybox.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, "skyboxDefault");
    skybox.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "skyboxDefault");
}

ES::Engine::Entity CreateSkyBox(ES::Engine::Core &core, const std::string_view texture_path,
                                const glm::vec3 &world_position, const glm::quat &world_rotation,
                                const glm::vec3 &skybox_dimensions)
{
    if (texture_path.empty())
        throw std::invalid_argument("Texture path cannot be empty");

    auto skybox_entity = CreateSkyBoxEntity(core, world_position, world_rotation, skybox_dimensions);

    auto &cubemap_resource_manager = core.GetResource<ES::Plugin::OpenGL::Resource::CubeMapManager>();

    const auto texture_file_path = std::filesystem::path(texture_path);
    if (!texture_file_path.has_filename())
        throw std::invalid_argument("Invalid texture path provided");

    const std::string texture_file_name = texture_file_path.stem().string();
    const std::string unique_resource_id = "cubemap_cross_" + texture_file_name;

    try
    {
        cubemap_resource_manager.Add(entt::hashed_string{unique_resource_id.c_str()}, texture_path.data());
        skybox_entity.AddComponent<ES::Plugin::OpenGL::Component::CubeMapHandle>(core, unique_resource_id.c_str());
        skybox_entity.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, unique_resource_id.c_str());

        AddSkyboxRenderingComponents(core, skybox_entity);
    }
    catch (const std::exception &loading_error)
    {
        throw std::runtime_error("Failed to load cross-layout texture: " + std::string(loading_error.what()));
    }

    return skybox_entity;
}

ES::Engine::Entity CreateSkyBox(ES::Engine::Core &core, const std::array<std::string, 6> &texture_paths,
                                const glm::vec3 &world_position, const glm::quat &world_rotation,
                                const glm::vec3 &skybox_dimensions)
{
    for (size_t path_index = 0; const auto &current_texture_path : texture_paths)
    {
        if (current_texture_path.empty())
            throw std::invalid_argument("Texture path at index " + std::to_string(path_index) + " cannot be empty");

        ++path_index;
    }

    auto skybox_entity = CreateSkyBoxEntity(core, world_position, world_rotation, skybox_dimensions);

    auto &cubemap_resource_manager = core.GetResource<ES::Plugin::OpenGL::Resource::CubeMapManager>();

    const auto first_texture_file_path = std::filesystem::path(texture_paths[0]);
    if (!first_texture_file_path.has_filename())
        throw std::invalid_argument("Invalid texture path provided at index 0");

    const std::string base_file_name = first_texture_file_path.stem().string();
    const std::string unique_resource_id = "cubemap_faces_" + base_file_name;

    try
    {
        cubemap_resource_manager.Add(entt::hashed_string{unique_resource_id.c_str()}, texture_paths);
        skybox_entity.AddComponent<ES::Plugin::OpenGL::Component::CubeMapHandle>(core, unique_resource_id.c_str());
        skybox_entity.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, unique_resource_id.c_str());

        AddSkyboxRenderingComponents(core, skybox_entity);
    }
    catch (const std::exception &loading_error)
    {
        throw std::runtime_error("Failed to load cubemap textures: " + std::string(loading_error.what()));
    }

    return skybox_entity;
}
