#include "OBJLoader.hpp"

namespace Plugin::Object {

OBJLoader::OBJLoader(const std::string &filepath, const std::string &mtlSearchPath)
{
    _reader_config.mtl_search_path = mtlSearchPath;

    if (!_reader.ParseFromFile(filepath, _reader_config))
    {
        if (!_reader.Error().empty())
        {
            throw OBJLoaderError(_reader.Error());
        }
        throw OBJLoaderError("Failed to load the OBJ file.");
    }

    if (!_reader.Warning().empty())
        throw OBJLoaderError(_reader.Warning());
}

Component::Mesh OBJLoader::GetMesh()
{
    Component::Mesh mesh{};

    auto &attrib = _reader.GetAttrib();
    auto &shapes = _reader.GetShapes();

    mesh.vertices.reserve(attrib.vertices.size() / 3u);
    mesh.normals.reserve(attrib.normals.size() / 3u);
    mesh.texCoords.reserve(attrib.texcoords.size() / 2u);
    mesh.indices.reserve(shapes.size() * 3u);

    for (size_t shape = 0u; shape < shapes.size(); ++shape)
    {
        size_t index_offset = 0u;

        for (size_t face = 0u; face < shapes[shape].mesh.num_face_vertices.size(); ++face)
        {
            size_t face_vertices = static_cast<size_t>(shapes[shape].mesh.num_face_vertices[face]);

            for (size_t vertex = 0u; vertex < face_vertices; ++vertex)
            {
                tinyobj::index_t idx = shapes[shape].mesh.indices[index_offset + vertex];

                if (idx.vertex_index >= 0)
                {
                    size_t vertex_index = static_cast<size_t>(idx.vertex_index);
                    float vertex_x = static_cast<float>(attrib.vertices[3u * vertex_index + 0u]);
                    float vertex_y = static_cast<float>(attrib.vertices[3u * vertex_index + 1u]);
                    float vertex_z = static_cast<float>(attrib.vertices[3u * vertex_index + 2u]);
                    mesh.vertices.emplace_back(vertex_x, vertex_y, vertex_z);
                }
                else
                {
                    mesh.vertices.emplace_back(0.0f, 0.0f, 0.0f);
                }

                if (idx.normal_index >= 0)
                {
                    size_t normal_index = static_cast<size_t>(idx.normal_index);
                    float normal_x = static_cast<float>(attrib.normals[3u * normal_index + 0u]);
                    float normal_y = static_cast<float>(attrib.normals[3u * normal_index + 1u]);
                    float normal_z = static_cast<float>(attrib.normals[3u * normal_index + 2u]);
                    mesh.normals.emplace_back(normal_x, normal_y, normal_z);
                }
                else
                {
                    mesh.normals.emplace_back(0.0f, 0.0f, 0.0f);
                }

                if (idx.texcoord_index >= 0)
                {
                    size_t texcoord_index = static_cast<size_t>(idx.texcoord_index);
                    float texcoord_x = static_cast<float>(attrib.texcoords[2u * texcoord_index + 0u]);
                    float texcoord_y = static_cast<float>(attrib.texcoords[2u * texcoord_index + 1u]);
                    mesh.texCoords.emplace_back(texcoord_x, texcoord_y);
                }
                else
                {
                    mesh.texCoords.emplace_back(0.0f, 0.0f);
                }

                mesh.indices.emplace_back(static_cast<uint32_t>(mesh.vertices.size() - 1u));
            }

            index_offset += face_vertices;
        }
    }

    return mesh;
}

} // namespace Plugin::Object
