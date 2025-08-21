#include "Object.pch.hpp"

#include "OBJLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace ES::Plugin::Object::Resource {

bool OBJLoader::loadModel(const std::string &path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals,
                          std::vector<glm::vec2> &texCoords, std::vector<uint32_t> &indices)
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
    normals.reserve(attrib.normals.size() / 3);
    texCoords.reserve(attrib.texcoords.size() / 2);
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
                vertices.emplace_back(vertex.pos);
                normals.emplace_back(vertex.normal);
                texCoords.emplace_back(vertex.texCoord);
            }

            indices.emplace_back(uniqueVertices[vertex]);
        }
    }

    return true;
}

bool OBJLoader::loadModel(const std::string &path, std::vector<Shape> &shape)
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

    shape.clear();
    shape.reserve(shapes.size());

    for (const auto &inShape : shapes)
    {
        Shape out{};
        std::unordered_map<Component::Vertex, uint32_t> uniqueVertices{};

        for (const auto &index : inShape.mesh.indices)
        {
            Component::Vertex vertex{};

            vertex.pos = {attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                          attrib.vertices[3 * index.vertex_index + 2]};

            if (!attrib.normals.empty() && index.normal_index >= 0)
            {
                vertex.normal = {attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1],
                                 attrib.normals[3 * index.normal_index + 2]};
            }

            if (!attrib.texcoords.empty() && index.texcoord_index >= 0)
            {
                vertex.texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                   VK_REVERSE_Y_TEX attrib.texcoords[2 * index.texcoord_index + 1]};
            }

            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(out.vertices.size());
                out.vertices.emplace_back(vertex.pos);
                out.normals.emplace_back(vertex.normal);
                out.texCoords.emplace_back(vertex.texCoord);
            }

            out.indices.emplace_back(uniqueVertices[vertex]);
        }

        shape.emplace_back(std::move(out));
    }

    return true;
}

} // namespace ES::Plugin::Object::Resource
