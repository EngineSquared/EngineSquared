#include "MeshSimplifier.hpp"
#include "MeshUtils.hpp"

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>

namespace Object::Utils {

namespace {

/**
 * @brief Spatial hash for efficient vertex proximity queries
 */
class SpatialHash {
  public:
    SpatialHash(float cellSize) : _cellSize(cellSize), _invCellSize(1.0f / cellSize) {}

    /**
     * @brief Get hash key for a position
     */
    int64_t GetKey(const glm::vec3 &pos) const
    {
        int32_t x = static_cast<int32_t>(std::floor(pos.x * _invCellSize));
        int32_t y = static_cast<int32_t>(std::floor(pos.y * _invCellSize));
        int32_t z = static_cast<int32_t>(std::floor(pos.z * _invCellSize));

        return (static_cast<int64_t>(x) * 73856093) ^ (static_cast<int64_t>(y) * 19349663) ^
               (static_cast<int64_t>(z) * 83492791);
    }

    /**
     * @brief Insert a vertex index at a position
     */
    void Insert(const glm::vec3 &pos, uint32_t index) { _cells[GetKey(pos)].push_back(index); }

    /**
     * @brief Get all vertices in the same cell and neighboring cells
     */
    std::vector<uint32_t> GetNearby(const glm::vec3 &pos) const
    {
        std::vector<uint32_t> result;

        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                for (int dz = -1; dz <= 1; ++dz)
                {
                    glm::vec3 offset(dx * _cellSize, dy * _cellSize, dz * _cellSize);
                    int64_t key = GetKey(pos + offset);

                    auto it = _cells.find(key);
                    if (it != _cells.end())
                    {
                        result.insert(result.end(), it->second.begin(), it->second.end());
                    }
                }
            }
        }

        return result;
    }

  private:
    float _cellSize;
    float _invCellSize;
    std::unordered_map<int64_t, std::vector<uint32_t>> _cells;
};

/**
 * @brief Find cluster representative using union-find
 */
class UnionFind {
  public:
    UnionFind(uint32_t size) : _parent(size), _rank(size, 0)
    {
        for (uint32_t i = 0; i < size; ++i)
        {
            _parent[i] = i;
        }
    }

    uint32_t Find(uint32_t x)
    {
        if (_parent[x] != x)
        {
            _parent[x] = Find(_parent[x]);
        }
        return _parent[x];
    }

    void Unite(uint32_t x, uint32_t y)
    {
        uint32_t rootX = Find(x);
        uint32_t rootY = Find(y);

        if (rootX == rootY)
            return;

        if (_rank[rootX] < _rank[rootY])
        {
            _parent[rootX] = rootY;
        }
        else if (_rank[rootX] > _rank[rootY])
        {
            _parent[rootY] = rootX;
        }
        else
        {
            _parent[rootY] = rootX;
            _rank[rootX]++;
        }
    }

  private:
    std::vector<uint32_t> _parent;
    std::vector<uint32_t> _rank;
};

/**
 * @brief Check if a triangle is degenerate (has zero area or duplicate vertices)
 */
bool IsTriangleDegenerate(uint32_t i0, uint32_t i1, uint32_t i2, const std::vector<glm::vec3> &vertices,
                          float epsilon = 1e-8f)
{
    if (i0 == i1 || i1 == i2 || i0 == i2)
        return true;

    const glm::vec3 &v0 = vertices[i0];
    const glm::vec3 &v1 = vertices[i1];
    const glm::vec3 &v2 = vertices[i2];

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 cross = glm::cross(edge1, edge2);

    return glm::length(cross) < epsilon;
}

} // namespace

SimplificationResult SimplifyMesh(const Component::Mesh &mesh, const SimplificationSettings &settings)
{
    SimplificationResult result;
    result.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());

    if (mesh.GetVertices().size() <= settings.targetVertexCount)
    {
        result.mesh = mesh;
        result.simplifiedVertexCount = result.originalVertexCount;
        result.wasSimplified = false;
        result.vertexMap.resize(result.originalVertexCount);
        for (uint32_t i = 0; i < result.originalVertexCount; ++i)
        {
            result.vertexMap[i] = i;
        }
        return result;
    }

    const auto &vertices = mesh.GetVertices();
    const auto &normals = mesh.GetNormals();
    const auto &texCoords = mesh.GetTexCoords();
    const auto &indices = mesh.GetIndices();

    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    for (const auto &v : vertices)
    {
        minBound = glm::min(minBound, v);
        maxBound = glm::max(maxBound, v);
    }
    glm::vec3 extent = maxBound - minBound;
    float meshDiagonal = glm::length(extent);

    float mergeDistance = settings.mergeDistance + (settings.aggressiveness * meshDiagonal * 0.01f);

    SpatialHash spatialHash(mergeDistance * 2.0f);
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        spatialHash.Insert(vertices[i], i);
    }

    UnionFind uf(static_cast<uint32_t>(vertices.size()));

    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        auto nearby = spatialHash.GetNearby(vertices[i]);
        for (uint32_t j : nearby)
        {
            if (j <= i)
                continue;

            float dist = glm::distance(vertices[i], vertices[j]);
            if (dist < mergeDistance)
            {
                uf.Unite(i, j);
            }
        }
    }

    std::unordered_map<uint32_t, uint32_t> clusterToNewIndex;
    std::vector<glm::vec3> newVertices;
    std::vector<glm::vec3> newNormals;
    std::vector<glm::vec2> newTexCoords;
    std::vector<glm::vec3> clusterSums;
    std::vector<glm::vec3> normalSums;
    std::vector<glm::vec2> texCoordSums;
    std::vector<uint32_t> clusterCounts;

    result.vertexMap.resize(vertices.size());

    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        uint32_t cluster = uf.Find(i);

        auto it = clusterToNewIndex.find(cluster);
        if (it == clusterToNewIndex.end())
        {
            uint32_t newIdx = static_cast<uint32_t>(newVertices.size());
            clusterToNewIndex[cluster] = newIdx;
            newVertices.push_back(vertices[i]);
            clusterSums.push_back(vertices[i]);

            if (!normals.empty())
            {
                newNormals.push_back(normals[i]);
                normalSums.push_back(normals[i]);
            }
            if (!texCoords.empty())
            {
                newTexCoords.push_back(texCoords[i]);
                texCoordSums.push_back(texCoords[i]);
            }
            clusterCounts.push_back(1);
            result.vertexMap[i] = newIdx;
        }
        else
        {
            uint32_t newIdx = it->second;
            clusterSums[newIdx] += vertices[i];
            if (!normals.empty())
                normalSums[newIdx] += normals[i];
            if (!texCoords.empty())
                texCoordSums[newIdx] += texCoords[i];
            clusterCounts[newIdx]++;
            result.vertexMap[i] = newIdx;
        }
    }

    for (uint32_t i = 0; i < newVertices.size(); ++i)
    {
        float invCount = 1.0f / static_cast<float>(clusterCounts[i]);
        newVertices[i] = clusterSums[i] * invCount;
        if (!normals.empty())
        {
            newNormals[i] = glm::normalize(normalSums[i]);
        }
        if (!texCoords.empty())
        {
            newTexCoords[i] = texCoordSums[i] * invCount;
        }
    }

    std::vector<uint32_t> newIndices;
    newIndices.reserve(indices.size());

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        uint32_t i0 = result.vertexMap[indices[i]];
        uint32_t i1 = result.vertexMap[indices[i + 1]];
        uint32_t i2 = result.vertexMap[indices[i + 2]];

        if (!IsTriangleDegenerate(i0, i1, i2, newVertices))
        {
            newIndices.push_back(i0);
            newIndices.push_back(i1);
            newIndices.push_back(i2);
        }
    }

    result.mesh.SetVertices(newVertices);
    result.mesh.SetIndices(newIndices);

    if (!texCoords.empty())
    {
        result.mesh.SetTexCoords(newTexCoords);
    }

    if (settings.recalculateNormals && !newVertices.empty() && !newIndices.empty())
    {
        std::vector<glm::vec3> recalcNormals(newVertices.size(), glm::vec3(0.0f));
        result.mesh.SetNormals(recalcNormals);
        RecalculateNormals(result.mesh);
    }
    else if (!newNormals.empty())
    {
        result.mesh.SetNormals(newNormals);
    }

    result.simplifiedVertexCount = static_cast<uint32_t>(newVertices.size());
    result.wasSimplified = true;

    return result;
}

SimplificationResult DeduplicateVertices(const Component::Mesh &mesh, float epsilon)
{
    SimplificationResult result;
    result.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());

    const auto &vertices = mesh.GetVertices();
    const auto &normals = mesh.GetNormals();
    const auto &texCoords = mesh.GetTexCoords();
    const auto &indices = mesh.GetIndices();

    if (vertices.empty())
    {
        result.mesh = mesh;
        result.simplifiedVertexCount = 0;
        result.wasSimplified = false;
        return result;
    }

    SpatialHash spatialHash(epsilon * 2.0f);
    std::vector<glm::vec3> newVertices;
    std::vector<glm::vec3> newNormals;
    std::vector<glm::vec2> newTexCoords;
    result.vertexMap.resize(vertices.size());

    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        const glm::vec3 &v = vertices[i];
        auto nearby = spatialHash.GetNearby(v);

        bool found = false;
        for (uint32_t j : nearby)
        {
            if (glm::distance(v, newVertices[j]) < epsilon)
            {
                result.vertexMap[i] = j;
                found = true;
                break;
            }
        }

        if (!found)
        {
            uint32_t newIdx = static_cast<uint32_t>(newVertices.size());
            result.vertexMap[i] = newIdx;
            newVertices.push_back(v);
            spatialHash.Insert(v, newIdx);

            if (!normals.empty())
            {
                newNormals.push_back(normals[i]);
            }
            if (!texCoords.empty())
            {
                newTexCoords.push_back(texCoords[i]);
            }
        }
    }

    std::vector<uint32_t> newIndices;
    newIndices.reserve(indices.size());

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        uint32_t i0 = result.vertexMap[indices[i]];
        uint32_t i1 = result.vertexMap[indices[i + 1]];
        uint32_t i2 = result.vertexMap[indices[i + 2]];

        if (!IsTriangleDegenerate(i0, i1, i2, newVertices))
        {
            newIndices.push_back(i0);
            newIndices.push_back(i1);
            newIndices.push_back(i2);
        }
    }

    result.mesh.SetVertices(newVertices);
    result.mesh.SetIndices(newIndices);

    if (!newNormals.empty())
    {
        result.mesh.SetNormals(newNormals);
    }
    if (!newTexCoords.empty())
    {
        result.mesh.SetTexCoords(newTexCoords);
    }

    result.simplifiedVertexCount = static_cast<uint32_t>(newVertices.size());
    result.wasSimplified = (result.simplifiedVertexCount < result.originalVertexCount);

    return result;
}

uint32_t EstimateSimplifiedVertexCount(const Component::Mesh &mesh, const SimplificationSettings &settings)
{
    const auto &vertices = mesh.GetVertices();

    if (vertices.size() <= settings.targetVertexCount)
    {
        return static_cast<uint32_t>(vertices.size());
    }

    glm::vec3 minBound(FLT_MAX);
    glm::vec3 maxBound(-FLT_MAX);
    for (const auto &v : vertices)
    {
        minBound = glm::min(minBound, v);
        maxBound = glm::max(maxBound, v);
    }
    glm::vec3 extent = maxBound - minBound;

    float cellSize = settings.mergeDistance * 2.0f;
    uint32_t cellsX = static_cast<uint32_t>(std::ceil(extent.x / cellSize)) + 1;
    uint32_t cellsY = static_cast<uint32_t>(std::ceil(extent.y / cellSize)) + 1;
    uint32_t cellsZ = static_cast<uint32_t>(std::ceil(extent.z / cellSize)) + 1;

    uint32_t maxCells = cellsX * cellsY * cellsZ;
    uint32_t estimated = std::min(static_cast<uint32_t>(vertices.size()), maxCells);

    estimated = static_cast<uint32_t>(estimated * (1.0f - settings.aggressiveness * 0.5f));

    return std::max(estimated, 100u);
}

} // namespace Object::Utils
