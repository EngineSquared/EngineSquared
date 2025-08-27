#include "Object.pch.hpp"

#include "OBJLoader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <filesystem>

namespace ES::Plugin::Object::Resource {

namespace {

bool validateAndLoadObj(const std::string &path, tinyobj::attrib_t &attrib, std::vector<tinyobj::shape_t> &shapes,
                        std::vector<tinyobj::material_t> &materials)
{
    std::string warn;
    std::string err;

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

    // Get the directory path for MTL files
    std::filesystem::path objPath(path);
    std::string mtlBaseDir = objPath.parent_path().string();

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), mtlBaseDir.c_str()))
    {
        ES::Utils::Log::Warn(warn + err);
        return false;
    }

    return true;
}

inline Component::Vertex buildVertex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &idx)
{
    Component::Vertex v{};

    v.pos = {attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1],
             attrib.vertices[3 * idx.vertex_index + 2]};

    if (!attrib.normals.empty() && idx.normal_index >= 0)
    {
        v.normal = {attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]};
    }

    if (!attrib.texcoords.empty() && idx.texcoord_index >= 0)
    {
        v.texCoord = {attrib.texcoords[2 * idx.texcoord_index + 0],
                      VK_REVERSE_Y_TEX attrib.texcoords[2 * idx.texcoord_index + 1]};
    }

    return v;
}

void appendShapeFlat(const tinyobj::attrib_t &attrib, const tinyobj::shape_t &inShape,
                     std::unordered_map<Component::Vertex, uint32_t> &uniqueVertices, std::vector<glm::vec3> &vertices,
                     std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texCoords, std::vector<uint32_t> &indices)
{
    for (const auto &idx : inShape.mesh.indices)
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

void appendShapeSeparated(const tinyobj::attrib_t &attrib, const tinyobj::shape_t &inShape, Shape &outShape)
{
    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices;

    uniqueVertices.reserve(inShape.mesh.indices.size());

    for (const auto &idx : inShape.mesh.indices)
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

} // namespace

bool OBJLoader::loadModel(const std::string &path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals,
                          std::vector<glm::vec2> &texCoords, std::vector<uint32_t> &indices)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    if (!validateAndLoadObj(path, attrib, shapes, materials))
        return false;

    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();

    vertices.reserve(attrib.vertices.size() / 3);
    normals.reserve(attrib.normals.size() / 3);
    texCoords.reserve(attrib.texcoords.size() / 2);
    indices.reserve(shapes.empty() ? 0 : shapes.front().mesh.indices.size() * shapes.size());

    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices;
    uniqueVertices.reserve(vertices.capacity());

    for (const auto &s : shapes)
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

    for (const auto &inShape : shapesIn)
    {
        Shape out{};
        out.vertices.reserve(inShape.mesh.indices.size());
        appendShapeSeparated(attrib, inShape, out);
        shape.emplace_back(std::move(out));
    }

    return true;
}

bool OBJLoader::loadModelWithFaces(const std::string &path, std::vector<glm::vec3> &vertices,
                                   std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texCoords,
                                   std::vector<uint32_t> &indices, std::vector<Component::Face> &faces)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    if (!validateAndLoadObj(path, attrib, shapes, materials))
        return false;

    vertices.clear();
    normals.clear();
    texCoords.clear();
    indices.clear();
    faces.clear();

    vertices.reserve(attrib.vertices.size() / 3);
    normals.reserve(attrib.normals.size() / 3);
    texCoords.reserve(attrib.texcoords.size() / 2);
    indices.reserve(shapes.empty() ? 0 : shapes.front().mesh.indices.size() * shapes.size());

    std::unordered_map<Component::Vertex, uint32_t> uniqueVertices;
    uniqueVertices.reserve(vertices.capacity());

    // Process each shape
    for (const auto &shape : shapes)
    {
        // Group faces by material
        std::unordered_map<int, std::vector<uint32_t>> materialToIndices;

        // Process each face (triangle) in the shape
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];
            int material_id = shape.mesh.material_ids[f];

            // Process vertices of this face
            std::vector<uint32_t> faceIndices;
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Component::Vertex vertex = buildVertex(attrib, idx);

                auto it = uniqueVertices.find(vertex);
                if (it == uniqueVertices.end())
                {
                    uint32_t newIndex = static_cast<uint32_t>(vertices.size());
                    uniqueVertices.emplace(vertex, newIndex);
                    vertices.emplace_back(vertex.pos);
                    normals.emplace_back(vertex.normal);
                    texCoords.emplace_back(vertex.texCoord);
                    faceIndices.emplace_back(newIndex);
                    indices.emplace_back(newIndex);
                }
                else
                {
                    faceIndices.emplace_back(it->second);
                    indices.emplace_back(it->second);
                }
            }

            // Add face indices to the material group
            if (materialToIndices.find(material_id) == materialToIndices.end())
            {
                materialToIndices[material_id] = std::vector<uint32_t>();
            }
            materialToIndices[material_id].insert(materialToIndices[material_id].end(), faceIndices.begin(),
                                                  faceIndices.end());

            index_offset += fv;
        }

        // Create Face objects from material groups
        for (const auto &materialGroup : materialToIndices)
        {
            int material_id = materialGroup.first;
            const std::vector<uint32_t> &faceIndices = materialGroup.second;

            // Get material name (use default if no material or material not found)
            std::string materialName = "default";
            if (material_id >= 0 && material_id < static_cast<int>(materials.size()))
            {
                const auto &material = materials[material_id];
                if (!material.diffuse_texname.empty())
                {
                    // Use the texture name as material identifier
                    materialName = material.diffuse_texname;
                }
                else
                {
                    // Use the material name itself
                    materialName = material.name.empty() ? "default" : material.name;
                }
            }

            // Create Face with material hash
            Component::Face face;
            face.indices = faceIndices;
            face.materialId = entt::hashed_string(materialName.c_str());
            faces.emplace_back(std::move(face));
        }
    }

    return true;
}

} // namespace ES::Plugin::Object::Resource
