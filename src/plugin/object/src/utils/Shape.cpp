#include "Shape.hpp"

void ES::Plugin::Object::Utils::CombineShapesIntoMesh(const std::vector<Shape> &shapes,
                                                          std::vector<glm::vec3> &outVertices,
                                                          std::vector<glm::vec3> &outNormals,
                                                          std::vector<glm::vec2> &outTexCoords,
                                                          std::vector<uint32_t> &outIndices)
{
    outVertices.clear();
    outNormals.clear();
    outTexCoords.clear();
    outIndices.clear();

    size_t vertexOffset = 0;

    for (const auto &shape : shapes)
    {
        outVertices.insert(outVertices.end(), shape.vertices.begin(), shape.vertices.end());
        outNormals.insert(outNormals.end(), shape.normals.begin(), shape.normals.end());
        outTexCoords.insert(outTexCoords.end(), shape.texCoords.begin(), shape.texCoords.end());

        for (const auto &index : shape.indices)
        {
            outIndices.push_back(index + static_cast<uint32_t>(vertexOffset));
        }

        vertexOffset += shape.vertices.size();
    }
}
