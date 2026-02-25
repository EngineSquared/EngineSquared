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
    explicit SpatialHash(float cellSize) : _cellSize(std::max(cellSize, 1e-6f)),
                                           _invCellSize(1.0f / _cellSize) {}

    /**
     * @brief Get hash key for a position
     */
    int64_t GetKey(const glm::vec3 &pos) const
    {
        auto x = static_cast<int32_t>(std::floor(pos.x * _invCellSize));
        auto y = static_cast<int32_t>(std::floor(pos.y * _invCellSize));
        auto z = static_cast<int32_t>(std::floor(pos.z * _invCellSize));

        return (static_cast<int64_t>(x) * 73856093) ^
               (static_cast<int64_t>(y) * 19349663) ^
               (static_cast<int64_t>(z) * 83492791);
    }

    /**
     * @brief Insert a vertex index at a position
     */
    void Insert(const glm::vec3 &pos, uint32_t index)
    {
        _cells[GetKey(pos)].push_back(index);
    }

    /**
     * @brief Get all vertices in the same cell and neighboring cells
     */
    std::vector<uint32_t> GetNearby(const glm::vec3 &pos) const
    {
        std::vector<uint32_t> result;

        static const std::array<glm::ivec3, 27> neighborOffsets = {
            glm::ivec3(-1, -1, -1), glm::ivec3(-1, -1, 0), glm::ivec3(-1, -1, 1),
            glm::ivec3(-1, 0, -1),  glm::ivec3(-1, 0, 0),  glm::ivec3(-1, 0, 1),
            glm::ivec3(-1, 1, -1),  glm::ivec3(-1, 1, 0),  glm::ivec3(-1, 1, 1),

            glm::ivec3(0, -1, -1),  glm::ivec3(0, -1, 0),  glm::ivec3(0, -1, 1),
            glm::ivec3(0, 0, -1),   glm::ivec3(0, 0, 0),   glm::ivec3(0, 0, 1),
            glm::ivec3(0, 1, -1),   glm::ivec3(0, 1, 0),   glm::ivec3(0, 1, 1),

            glm::ivec3(1, -1, -1),  glm::ivec3(1, -1, 0),  glm::ivec3(1, -1, 1),
            glm::ivec3(1, 0, -1),   glm::ivec3(1, 0, 0),   glm::ivec3(1, 0, 1),
            glm::ivec3(1, 1, -1),   glm::ivec3(1, 1, 0),   glm::ivec3(1, 1, 1),
        };

        for (const auto &off : neighborOffsets)
        {
            glm::vec3 offset(off.x * _cellSize, off.y * _cellSize, off.z * _cellSize);
            int64_t key = GetKey(pos + offset);
            auto it = _cells.find(key);
            if (it != _cells.end())
                result.insert(result.end(), it->second.begin(), it->second.end());
        }

        return result;
    }

  private:
    float _cellSize;
    float _invCellSize;
    std::unordered_map<int64_t, std::vector<uint32_t>> _cells;
};

/**
 * @brief Find cluster representative using union-find with path compression
 */
class UnionFind {
  public:
    explicit UnionFind(uint32_t size) : _parent(size), _rank(size, 0)
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
            _parent[x] = Find(_parent[x]); // Path compression
        }
        return _parent[x];
    }

    void Unite(uint32_t x, uint32_t y)
    {
        uint32_t rootX = Find(x);
        uint32_t rootY = Find(y);

        if (rootX == rootY)
            return;

        // Union by rank
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
bool IsTriangleDegenerate(uint32_t i0, uint32_t i1, uint32_t i2,
                          const std::vector<glm::vec3> &vertices,
                          float epsilon = 1e-8f)
{
    if (i0 == i1 || i1 == i2 || i0 == i2)
        return true;

    if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size())
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

/**
 * @brief Helper: compute mesh bounds
 */
static void ComputeBounds(const std::vector<glm::vec3> &vertices,
                          glm::vec3 &minBound, glm::vec3 &maxBound)
{
    minBound = glm::vec3(FLT_MAX);
    maxBound = glm::vec3(-FLT_MAX);

    for (const auto &v : vertices)
    {
        minBound = glm::min(minBound, v);
        maxBound = glm::max(maxBound, v);
    }
}

/**
 * @brief Compute adaptive merge distance based on mesh size and settings
 */
static float ComputeMergeDistance(const std::vector<glm::vec3> &vertices,
                                  const SimplificationSettings &settings)
{
    glm::vec3 minBound, maxBound;
    ComputeBounds(vertices, minBound, maxBound);
    glm::vec3 extent = maxBound - minBound;
    float meshDiagonal = glm::length(extent);

    // Base merge distance + adaptive component based on mesh size and aggressiveness
    return settings.mergeDistance + (settings.aggressiveness * meshDiagonal * 0.01f);
}

/**
 * @brief Cluster vertices using spatial hash and return UnionFind
 */
static UnionFind ClusterVertices(const std::vector<glm::vec3> &vertices,
                                 float mergeDistance,
                                 const SpatialHash &spatialHash)
{
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

    return uf;
}

/**
 * @brief Collapse clusters into centroids and compute new vertex arrays
 */
struct CollapseResult {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> vertexMap;
};

static CollapseResult CollapseClusters(const std::vector<glm::vec3> &vertices,
                                       const std::vector<glm::vec3> &normals,
                                       const std::vector<glm::vec2> &texCoords,
                                       UnionFind &uf)
{
    CollapseResult res;

    std::unordered_map<uint32_t, uint32_t> clusterToNewIndex;
    std::vector<glm::vec3> clusterSums;
    std::vector<glm::vec3> normalSums;
    std::vector<glm::vec2> texCoordSums;
    std::vector<uint32_t> clusterCounts;

    res.vertexMap.resize(vertices.size());

    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        uint32_t cluster = uf.Find(i);
        auto it = clusterToNewIndex.find(cluster);
        if (it == clusterToNewIndex.end())
        {
            auto newIdx = static_cast<uint32_t>(res.vertices.size());
            clusterToNewIndex[cluster] = newIdx;
            res.vertices.push_back(vertices[i]);
            clusterSums.push_back(vertices[i]);

            if (!normals.empty() && i < normals.size())
            {
                res.normals.push_back(normals[i]);
                normalSums.push_back(normals[i]);
            }
            if (!texCoords.empty() && i < texCoords.size())
            {
                res.texCoords.push_back(texCoords[i]);
                texCoordSums.push_back(texCoords[i]);
            }
            clusterCounts.push_back(1);
            res.vertexMap[i] = newIdx;
        }
        else
        {
            uint32_t newIdx = it->second;
            clusterSums[newIdx] += vertices[i];
            if (!normals.empty() && i < normals.size())
                normalSums[newIdx] += normals[i];
            if (!texCoords.empty() && i < texCoords.size())
                texCoordSums[newIdx] += texCoords[i];
            clusterCounts[newIdx]++;
            res.vertexMap[i] = newIdx;
        }
    }

    // Compute centroids
    for (uint32_t i = 0; i < res.vertices.size(); ++i)
    {
        float invCount = 1.0f / static_cast<float>(clusterCounts[i]);
        res.vertices[i] = clusterSums[i] * invCount;
        if (!res.normals.empty() && i < res.normals.size())
        {
            glm::vec3 avgNormal = normalSums[i] * invCount;
            float len = glm::length(avgNormal);
            res.normals[i] = len > 1e-6f ? avgNormal / len : glm::vec3(0.0f, 1.0f, 0.0f);
        }
        if (!res.texCoords.empty() && i < res.texCoords.size())
        {
            res.texCoords[i] = texCoordSums[i] * invCount;
        }
    }

    return res;
}

SimplificationResult SimplifyMesh(const Component::Mesh &mesh,
                                  const SimplificationSettings &settings)
{
    SimplificationResult result;
    result.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());

    // If already under target, return unchanged
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

    float mergeDistance = ComputeMergeDistance(vertices, settings);

    // Build spatial hash
    SpatialHash spatialHash(mergeDistance * 2.0f);
    for (uint32_t i = 0; i < vertices.size(); ++i)
        spatialHash.Insert(vertices[i], i);

    // Cluster vertices
    UnionFind uf = ClusterVertices(vertices, mergeDistance, spatialHash);

    // Collapse clusters
    auto collapseRes = CollapseClusters(vertices, normals, texCoords, uf);
    result.vertexMap = std::move(collapseRes.vertexMap);

    // Rebuild indices, removing degenerate triangles
    std::vector<uint32_t> newIndices;
    newIndices.reserve(indices.size());

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        uint32_t i0 = result.vertexMap[indices[i]];
        uint32_t i1 = result.vertexMap[indices[i + 1]];
        uint32_t i2 = result.vertexMap[indices[i + 2]];

        if (!IsTriangleDegenerate(i0, i1, i2, collapseRes.vertices))
        {
            newIndices.push_back(i0);
            newIndices.push_back(i1);
            newIndices.push_back(i2);
        }
    }

    // Build result mesh
    result.mesh.SetVertices(collapseRes.vertices);
    result.mesh.SetIndices(newIndices);

    if (!texCoords.empty())
    {
        result.mesh.SetTexCoords(collapseRes.texCoords);
    }

    // Recalculate normals if requested
    if (settings.recalculateNormals && !collapseRes.vertices.empty() && !newIndices.empty())
    {
        std::vector<glm::vec3> recalcNormals(collapseRes.vertices.size(), glm::vec3(0.0f));
        result.mesh.SetNormals(recalcNormals);
        RecalculateNormals(result.mesh);
    }
    else if (!collapseRes.normals.empty())
    {
        result.mesh.SetNormals(collapseRes.normals);
    }

    result.simplifiedVertexCount = static_cast<uint32_t>(collapseRes.vertices.size());
    result.wasSimplified = (result.simplifiedVertexCount < result.originalVertexCount) ||
                           (newIndices.size() != indices.size());

    return result;
}

SimplificationResult SimplifyMeshToTarget(const Component::Mesh &mesh,
                                          uint32_t targetVertexCount,
                                          uint32_t maxIterations)
{
    // If already under target, return unchanged
    if (mesh.GetVertices().size() <= targetVertexCount)
    {
        SimplificationResult result;
        result.mesh = mesh;
        result.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());
        result.simplifiedVertexCount = result.originalVertexCount;
        result.wasSimplified = false;
        result.vertexMap.resize(result.originalVertexCount);
        for (uint32_t i = 0; i < result.originalVertexCount; ++i)
            result.vertexMap[i] = i;
        return result;
    }

    // Start with base settings
    SimplificationSettings settings = SimplificationSettings::ForSoftBody(targetVertexCount);
    Component::Mesh currentMesh = mesh;
    SimplificationResult bestResult;
    bestResult.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());

    for (uint32_t iter = 0; iter < maxIterations; ++iter)
    {
        auto result = SimplifyMesh(currentMesh, settings);

        if (result.simplifiedVertexCount <= targetVertexCount)
        {
            // Target reached
            result.originalVertexCount = bestResult.originalVertexCount;
            return result;
        }

        // Not enough reduction, increase aggressiveness
        if (result.simplifiedVertexCount >= currentMesh.GetVertices().size() * 0.95f)
        {
            // No significant progress, increase merge distance more aggressively
            settings.mergeDistance *= 2.0f;
            settings.aggressiveness = std::min(settings.aggressiveness + 0.15f, 1.0f);
        }
        else
        {
            // Some progress, moderate increase
            settings.mergeDistance *= 1.5f;
            settings.aggressiveness = std::min(settings.aggressiveness + 0.1f, 1.0f);
        }

        currentMesh = result.mesh;
        bestResult = result;
        bestResult.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());
    }

    return bestResult;
}

SimplificationResult SimplifyMeshByVoxelization(const Component::Mesh &mesh,
                                                 uint32_t targetVertexCount)
{
    SimplificationResult result;
    result.originalVertexCount = static_cast<uint32_t>(mesh.GetVertices().size());

    const auto &vertices = mesh.GetVertices();
    const auto &normals = mesh.GetNormals();
    const auto &texCoords = mesh.GetTexCoords();
    const auto &indices = mesh.GetIndices();

    if (vertices.empty() || vertices.size() <= targetVertexCount)
    {
        result.mesh = mesh;
        result.simplifiedVertexCount = result.originalVertexCount;
        result.wasSimplified = false;
        result.vertexMap.resize(result.originalVertexCount);
        for (uint32_t i = 0; i < result.originalVertexCount; ++i)
            result.vertexMap[i] = i;
        return result;
    }

    // Compute bounding box
    glm::vec3 minBound, maxBound;
    ComputeBounds(vertices, minBound, maxBound);
    glm::vec3 extent = maxBound - minBound;

    // Calculate cell size to achieve approximately targetVertexCount cells
    // We want cellsX * cellsY * cellsZ ≈ targetVertexCount
    // Assuming uniform distribution, use cube root
    float meshVolume = extent.x * extent.y * extent.z;
    if (meshVolume < 1e-10f)
    {
        // Degenerate mesh (flat or line)
        result.mesh = mesh;
        result.simplifiedVertexCount = result.originalVertexCount;
        result.wasSimplified = false;
        return result;
    }

    // Cell size = cube root of (volume / targetVertexCount)
    float cellSize = std::cbrt(meshVolume / static_cast<float>(targetVertexCount));
    cellSize = std::max(cellSize, 1e-4f); // Minimum cell size

    // Calculate grid dimensions
    uint32_t cellsX = std::max(1u, static_cast<uint32_t>(std::ceil(extent.x / cellSize)));
    uint32_t cellsY = std::max(1u, static_cast<uint32_t>(std::ceil(extent.y / cellSize)));
    uint32_t cellsZ = std::max(1u, static_cast<uint32_t>(std::ceil(extent.z / cellSize)));

    // Hash function for cell coordinates
    auto cellHash = [cellsY, cellsZ](uint32_t x, uint32_t y, uint32_t z) -> uint64_t {
        return static_cast<uint64_t>(x) * cellsY * cellsZ + 
               static_cast<uint64_t>(y) * cellsZ + z;
    };

    // Map from cell hash to: (sum of positions, sum of normals, sum of texcoords, count, new index)
    struct CellData {
        glm::vec3 posSum = glm::vec3(0.0f);
        glm::vec3 normalSum = glm::vec3(0.0f);
        glm::vec2 texCoordSum = glm::vec2(0.0f);
        uint32_t count = 0;
        uint32_t newIndex = UINT32_MAX;
    };
    std::unordered_map<uint64_t, CellData> cells;

    // Assign each vertex to a cell
    result.vertexMap.resize(vertices.size());
    float invCellSize = 1.0f / cellSize;

    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        const glm::vec3 &v = vertices[i];
        
        // Calculate cell coordinates
        uint32_t cx = static_cast<uint32_t>(std::floor((v.x - minBound.x) * invCellSize));
        uint32_t cy = static_cast<uint32_t>(std::floor((v.y - minBound.y) * invCellSize));
        uint32_t cz = static_cast<uint32_t>(std::floor((v.z - minBound.z) * invCellSize));
        
        // Clamp to grid bounds
        cx = std::min(cx, cellsX - 1);
        cy = std::min(cy, cellsY - 1);
        cz = std::min(cz, cellsZ - 1);

        uint64_t key = cellHash(cx, cy, cz);
        CellData &cell = cells[key];
        cell.posSum += v;
        if (!normals.empty() && i < normals.size())
            cell.normalSum += normals[i];
        if (!texCoords.empty() && i < texCoords.size())
            cell.texCoordSum += texCoords[i];
        cell.count++;
    }

    // Create new vertices from cell centroids
    std::vector<glm::vec3> newVertices;
    std::vector<glm::vec3> newNormals;
    std::vector<glm::vec2> newTexCoords;
    newVertices.reserve(cells.size());

    for (auto &[key, cell] : cells)
    {
        cell.newIndex = static_cast<uint32_t>(newVertices.size());
        float invCount = 1.0f / static_cast<float>(cell.count);
        newVertices.push_back(cell.posSum * invCount);
        
        if (!normals.empty())
        {
            glm::vec3 avgNormal = cell.normalSum * invCount;
            float len = glm::length(avgNormal);
            newNormals.push_back(len > 1e-6f ? avgNormal / len : glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (!texCoords.empty())
        {
            newTexCoords.push_back(cell.texCoordSum * invCount);
        }
    }

    // Build vertex map (original index -> new index)
    for (uint32_t i = 0; i < vertices.size(); ++i)
    {
        const glm::vec3 &v = vertices[i];
        
        uint32_t cx = static_cast<uint32_t>(std::floor((v.x - minBound.x) * invCellSize));
        uint32_t cy = static_cast<uint32_t>(std::floor((v.y - minBound.y) * invCellSize));
        uint32_t cz = static_cast<uint32_t>(std::floor((v.z - minBound.z) * invCellSize));
        cx = std::min(cx, cellsX - 1);
        cy = std::min(cy, cellsY - 1);
        cz = std::min(cz, cellsZ - 1);

        uint64_t key = cellHash(cx, cy, cz);
        result.vertexMap[i] = cells[key].newIndex;
    }

    // Rebuild indices, removing degenerate triangles
    std::vector<uint32_t> newIndices;
    newIndices.reserve(indices.size());

    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        uint32_t i0 = result.vertexMap[indices[i]];
        uint32_t i1 = result.vertexMap[indices[i + 1]];
        uint32_t i2 = result.vertexMap[indices[i + 2]];

        // Skip degenerate triangles
        if (i0 != i1 && i1 != i2 && i0 != i2)
        {
            newIndices.push_back(i0);
            newIndices.push_back(i1);
            newIndices.push_back(i2);
        }
    }

    // Build result mesh
    result.mesh.SetVertices(newVertices);
    result.mesh.SetIndices(newIndices);
    if (!newNormals.empty())
        result.mesh.SetNormals(newNormals);
    if (!newTexCoords.empty())
        result.mesh.SetTexCoords(newTexCoords);

    // Recalculate normals for better quality
    if (!newVertices.empty() && !newIndices.empty())
    {
        RecalculateNormals(result.mesh);
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
            if (j < newVertices.size() && glm::distance(v, newVertices[j]) < epsilon)
            {
                result.vertexMap[i] = j;
                found = true;
                break;
            }
        }

        if (!found)
        {
            auto newIdx = static_cast<uint32_t>(newVertices.size());
            result.vertexMap[i] = newIdx;
            newVertices.push_back(v);
            spatialHash.Insert(v, newIdx);

            if (!normals.empty() && i < normals.size())
            {
                newNormals.push_back(normals[i]);
            }
            if (!texCoords.empty() && i < texCoords.size())
            {
                newTexCoords.push_back(texCoords[i]);
            }
        }
    }

    // Rebuild indices
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

uint32_t EstimateSimplifiedVertexCount(const Component::Mesh &mesh,
                                       const SimplificationSettings &settings)
{
    const auto &vertices = mesh.GetVertices();

    if (vertices.size() <= settings.targetVertexCount)
    {
        return static_cast<uint32_t>(vertices.size());
    }

    glm::vec3 minBound, maxBound;
    ComputeBounds(vertices, minBound, maxBound);
    glm::vec3 extent = maxBound - minBound;

    float cellSize = settings.mergeDistance * 2.0f;
    uint32_t cellsX = static_cast<uint32_t>(std::ceil(extent.x / cellSize)) + 1;
    uint32_t cellsY = static_cast<uint32_t>(std::ceil(extent.y / cellSize)) + 1;
    uint32_t cellsZ = static_cast<uint32_t>(std::ceil(extent.z / cellSize)) + 1;

    uint64_t maxCells64 = static_cast<uint64_t>(cellsX) * cellsY * cellsZ;
    auto maxCells = static_cast<uint32_t>(std::min(maxCells64, static_cast<uint64_t>(UINT32_MAX)));
    uint32_t estimated = std::min(static_cast<uint32_t>(vertices.size()), maxCells);

    // Apply aggressiveness factor
    estimated = static_cast<uint32_t>(estimated * (1.0f - settings.aggressiveness * 0.5f));

    return std::max(estimated, 100u);
}

} // namespace Object::Utils
