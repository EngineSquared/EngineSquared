#include "Object.pch.hpp"

#include "OBJLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace ES::Plugin::Object::Resource {

namespace {

bool validateAndLoadObj(const std::string& path,
                        tinyobj::attrib_t& attrib,
                        std::vector<tinyobj::shape_t>& shapes,
                        std::vector<tinyobj::material_t>& materials)
{
    std::string warn, err;

    if (path.empty())
    {
        ES::Utils::Log::Warn("The path is empty.");
        return false;
    }
    if (!path.ends_with(".obj"))
    {
        ES::Utils::Log::Warn("The file is not a .obj file.");
        return false;
    }
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
    {
        ES::Utils::Log::Warn(warn + err);
        return false;
    }
    return true;
}

inline Component::Vertex buildVertex(const tinyobj::attrib_t& attrib, const tinyobj::index_t& idx)
{
    Component::Vertex v{};

    v.pos = {
        attrib.vertices[3 * idx.vertex_index + 0],
        attrib.vertices[3 * idx.vertex_index + 1],
        attrib.vertices[3 * idx.vertex_index + 2]};

    if (!attrib.normals.empty() && idx.normal_index >= 0)
    {
        v.normal = {
            attrib.normals[3 * idx.normal_index + 0],
            attrib.normals[3 * idx.normal_index + 1],
            attrib.normals[3 * idx.normal_index + 2]};
    }

    // UV (si disponible)
    if (!attrib.texcoords.empty() && idx.texcoord_index >= 0)
    {
        v.texCoord = {
            attrib.texcoords[2 * idx.texcoord_index + 0],
            VK_REVERSE_Y_TEX attrib.texcoords[2 * idx.texcoord_index + 1]};
    }

    return v;
}

void appendShapeFlat(const tinyobj::attrib_t& attrib,
                     const tinyobj::shape_t& inShape,
                     std::unordered_map<Component::Vertex, uint32_t>& uniqueVertices,
                     std::vector<glm::vec3>& vertices,
                     std::vector<glm::vec3>& normals,
                     std::vector<glm::vec2>& texCoords,
                     std::vector<uint32_t>& indices)
{
    for (const auto& idx : inShape.mesh.indices)
    {
        Component::Vertex v = buildVertex(attrib, idx);

        auto it = uniqueVertices.find(v);
        if (it == uniqueVertices.end())
        {
            uint32_t newIndex = static_cast<uint32_t>(vertices.size());
            uniqueVertices.emplace(v, newIndex);
            vertices.emplace_back(v.pos);
            normals.emplace_back(v.normal);
            texCoords.emplace_back(v.texCoord);
            indices.emplace_back(newIndex);
        }
        else
        {
            indices.emplace_back(it->second);
        }
    }
}

void appendShapeSeparated(const tinyobj::attrib_t& attrib,
                          const tinyobj::shape_t& inShape,
                          Shape& outShape)
{
    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices;
    uniqueVertices.reserve(inShape.mesh.indices.size());

    for (const auto& idx : inShape.mesh.indices)
    {
        Component::Vertex v = buildVertex(attrib, idx);

        auto it = uniqueVertices.find(v);
        if (it == uniqueVertices.end())
        {
            uint32_t newIndex = static_cast<uint32_t>(outShape.vertices.size());
            uniqueVertices.emplace(v, newIndex);
            outShape.vertices.emplace_back(v.pos);
            outShape.normals.emplace_back(v.normal);
            outShape.texCoords.emplace_back(v.texCoord);
            outShape.indices.emplace_back(newIndex);
        }
        else
        {
            outShape.indices.emplace_back(it->second);
        }
    }
}

} // anonymous namespace

bool OBJLoader::loadModel(const std::string &path,
                          std::vector<glm::vec3> &vertices,
                          std::vector<glm::vec3> &normals,
                          std::vector<glm::vec2> &texCoords,
                          std::vector<uint32_t> &indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    if (!validateAndLoadObj(path, attrib, shapes, materials))
        return false;

    vertices.clear(); normals.clear(); texCoords.clear(); indices.clear();

    vertices.reserve(attrib.vertices.size() / 3);
    normals.reserve(attrib.normals.size() / 3);
    texCoords.reserve(attrib.texcoords.size() / 2);
    indices.reserve(shapes.empty() ? 0 : shapes.front().mesh.indices.size() * shapes.size());

    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices;
    uniqueVertices.reserve(vertices.capacity());

    for (const auto& s : shapes)
        appendShapeFlat(attrib, s, uniqueVertices, vertices, normals, texCoords, indices);

    return true;
}

bool OBJLoader::loadModel(const std::string &path, std::vector<Shape> &shape)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapesIn;
    std::vector<tinyobj::material_t> materials;

    if (!validateAndLoadObj(path, attrib, shapesIn, materials))
        return false;

    shape.clear();
    shape.reserve(shapesIn.size());

    for (const auto& inShape : shapesIn)
    {
        Shape out{};
        out.vertices.reserve(inShape.mesh.indices.size()); // estimation
        appendShapeSeparated(attrib, inShape, out);
        shape.emplace_back(std::move(out));
    }

    return true;
}

} // namespace ES::Plugin::Object::Resource
