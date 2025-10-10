// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust triangulation. Requires C++11
// #define TINYOBJLOADER_USE_MAPBOX_EARCUT
#define TINYOBJLOADER_IMPLEMENTATION
#include "OBJLoader.hpp"

namespace Plugin::Object {

OBJLoader::OBJLoader(const std::string &filepath, const std::string &mtlSearchPath)
{
    if (filepath.empty())
        throw OBJLoaderError("The path is empty.");

    if (!filepath.ends_with(".obj"))
        throw OBJLoaderError("The file is not a .obj file.");

    _reader_config.mtl_search_path = mtlSearchPath;

    if (!_reader.ParseFromFile(filepath, _reader_config))
    {
        if (!_reader.Error().empty())
            throw OBJLoaderError(_reader.Error());

        throw OBJLoaderError("Failed to load the OBJ file.");
    }

    if (!_reader.Warning().empty())
        throw OBJLoaderError(_reader.Warning());
}

Component::Mesh OBJLoader::GetMesh()
{
    if (!_mesh.vertices.empty())
        return _mesh;

    const auto &attrib = _reader.GetAttrib();
    const auto &shapes = _reader.GetShapes();

    _mesh.vertices.reserve(attrib.vertices.size() / 3u);
    _mesh.normals.reserve(attrib.normals.size() / 3u);
    _mesh.texCoords.reserve(attrib.texcoords.size() / 2u);
    _mesh.indices.reserve(shapes.size() * 3u);

    for (size_t shape = 0u; shape < shapes.size(); ++shape)
    {
        size_t index_offset = 0u;

        for (size_t face = 0u; face < shapes[shape].mesh.num_face_vertices.size(); ++face)
        {
            auto face_vertices = static_cast<size_t>(shapes[shape].mesh.num_face_vertices[face]);

            ProcessMeshFace(_mesh, shapes, shape, face_vertices, index_offset);

            index_offset += face_vertices;
        }
    }

    return _mesh;
}

std::vector<Resource::Shape> OBJLoader::GetShapes()
{
    if (!_shapes.empty())
        return _shapes;

    const auto &attrib = _reader.GetAttrib();
    const auto &shapes = _reader.GetShapes();

    _shapes.reserve(shapes.size());

    for (size_t shape = 0u; shape < shapes.size(); ++shape)
    {
        Resource::Shape shapeResource;
        Component::Mesh &mesh = shapeResource.mesh;

        mesh.vertices.reserve(attrib.vertices.size() / 3u);
        mesh.normals.reserve(attrib.normals.size() / 3u);
        mesh.texCoords.reserve(attrib.texcoords.size() / 2u);
        mesh.indices.reserve(shapes[shape].mesh.indices.size());

        size_t index_offset = 0u;

        for (size_t face = 0u; face < shapes[shape].mesh.num_face_vertices.size(); ++face)
        {
            auto face_vertices = static_cast<size_t>(shapes[shape].mesh.num_face_vertices[face]);

            ProcessMeshFace(mesh, shapes, shape, face_vertices, index_offset);

            index_offset += face_vertices;
        }

        int material_id = shapes[shape].mesh.material_ids.empty() ? -1 : shapes[shape].mesh.material_ids[0];

        if (material_id >= 0 && static_cast<size_t>(material_id) < _reader.GetMaterials().size())
        {
            const auto &mat = _reader.GetMaterials()[material_id];
            SetMaterialProperties(shapeResource.material, mat);
        }

        _shapes.emplace_back(std::move(shapeResource));
    }

    return _shapes;
}

std::vector<Component::Material> OBJLoader::GetMaterials()
{
    if (!_materials.empty())
        return _materials;

    const auto &materials = _reader.GetMaterials();
    _materials.reserve(materials.size());

    for (const auto &mat : materials)
    {
        Component::Material material;
        SetMaterialProperties(material, mat);

        _materials.emplace_back(std::move(material));
    }

    return _materials;
}

void OBJLoader::ProcessMeshFace(Component::Mesh &mesh, const std::vector<tinyobj::shape_t> &shapes, size_t shape,
                                size_t face_vertices, size_t &index_offset) noexcept
{
    const auto &attrib = _reader.GetAttrib();

    for (size_t vertex = 0u; vertex < face_vertices; ++vertex)
    {
        tinyobj::index_t idx = shapes[shape].mesh.indices[index_offset + vertex];

        if (idx.vertex_index >= 0)
        {
            auto vertex_index = static_cast<size_t>(idx.vertex_index);
            auto vertex_x = static_cast<float>(attrib.vertices[3u * vertex_index + 0u]);
            auto vertex_y = static_cast<float>(attrib.vertices[3u * vertex_index + 1u]);
            auto vertex_z = static_cast<float>(attrib.vertices[3u * vertex_index + 2u]);
            mesh.vertices.emplace_back(vertex_x, vertex_y, vertex_z);
        }
        else
        {
            mesh.vertices.emplace_back(0.0f, 0.0f, 0.0f);
        }

        if (idx.normal_index >= 0)
        {
            auto normal_index = static_cast<size_t>(idx.normal_index);
            auto normal_x = static_cast<float>(attrib.normals[3u * normal_index + 0u]);
            auto normal_y = static_cast<float>(attrib.normals[3u * normal_index + 1u]);
            auto normal_z = static_cast<float>(attrib.normals[3u * normal_index + 2u]);
            mesh.normals.emplace_back(normal_x, normal_y, normal_z);
        }
        else
        {
            mesh.normals.emplace_back(0.0f, 0.0f, 0.0f);
        }

        if (idx.texcoord_index >= 0)
        {
            auto texcoord_index = static_cast<size_t>(idx.texcoord_index);
            auto texcoord_x = static_cast<float>(attrib.texcoords[2u * texcoord_index + 0u]);
            auto texcoord_y = static_cast<float>(attrib.texcoords[2u * texcoord_index + 1u]);
            mesh.texCoords.emplace_back(texcoord_x, texcoord_y);
        }
        else
        {
            mesh.texCoords.emplace_back(0.0f, 0.0f);
        }

        mesh.indices.emplace_back(static_cast<uint32_t>(mesh.vertices.size() - 1u));
    }
}

void OBJLoader::SetMaterialProperties(Component::Material &material, const tinyobj::material_t &mat) noexcept
{
    material.name = mat.name;
    material.ambient = glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
    material.diffuse = glm::vec3(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
    material.specular = glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]);
    material.transmittance = glm::vec3(mat.transmittance[0], mat.transmittance[1], mat.transmittance[2]);
    material.emission = glm::vec3(mat.emission[0], mat.emission[1], mat.emission[2]);
    material.shininess = mat.shininess;
    material.ior = mat.ior;
    material.dissolve = mat.dissolve;
}

} // namespace Plugin::Object
