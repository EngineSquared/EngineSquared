#include "OBJLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Logger.hpp"

namespace ES::Plugin::Object::Resource {

bool OBJLoader::loadModel(const std::string &path, std::vector<Component::Vertex> &vertices,
                          std::vector<uint32_t> &indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;

    if (path.empty())
    {
        ES::Utils::Log::Warn("The path is empty.");
        return false;
    }

    else if (path.ends_with(".obj") == false)
    {
        ES::Utils::Log::Warn("The file is not a .obj file.");
        return false;
    }

    else if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        ES::Utils::Log::Warn(warn + err);
        return false;
    }

    vertices.reserve(attrib.vertices.size() / 3);
    indices.reserve(attrib.vertices.size());

    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices{};

    for (const auto &shape : shapes)
    {
        for (const auto &index : shape.mesh.indices)
        {
            Component::Vertex vertex{};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            if (attrib.normals.size() > 0)
            {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            if (attrib.texcoords.size() > 0)
            {
                vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   VK_REVERSE_Y_TEX attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.emplace_back(vertex);
            }

            indices.emplace_back(uniqueVertices[vertex]);
        }
    }

    return true;
}

} // namespace ES::Plugin::Object::Resource
