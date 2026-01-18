#include "BoxGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

Component::Mesh GenerateBoxMesh(float width, float height, float depth)
{
    Component::Mesh mesh;

    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;
    float halfDepth = depth * 0.5f;

    mesh.SetVertices({
        // Front face (+Z)
        {-halfWidth, -halfHeight, halfDepth },
        {halfWidth,  -halfHeight, halfDepth },
        {halfWidth,  halfHeight,  halfDepth },
        {-halfWidth, halfHeight,  halfDepth },

        // Back face (-Z)
        {halfWidth,  -halfHeight, -halfDepth},
        {-halfWidth, -halfHeight, -halfDepth},
        {-halfWidth, halfHeight,  -halfDepth},
        {halfWidth,  halfHeight,  -halfDepth},

        // Top face (+Y)
        {-halfWidth, halfHeight,  halfDepth },
        {halfWidth,  halfHeight,  halfDepth },
        {halfWidth,  halfHeight,  -halfDepth},
        {-halfWidth, halfHeight,  -halfDepth},

        // Bottom face (-Y)
        {-halfWidth, -halfHeight, -halfDepth},
        {halfWidth,  -halfHeight, -halfDepth},
        {halfWidth,  -halfHeight, halfDepth },
        {-halfWidth, -halfHeight, halfDepth },

        // Right face (+X)
        {halfWidth,  -halfHeight, halfDepth },
        {halfWidth,  -halfHeight, -halfDepth},
        {halfWidth,  halfHeight,  -halfDepth},
        {halfWidth,  halfHeight,  halfDepth },

        // Left face (-X)
        {-halfWidth, -halfHeight, -halfDepth},
        {-halfWidth, -halfHeight, halfDepth },
        {-halfWidth, halfHeight,  halfDepth },
        {-halfWidth, halfHeight,  -halfDepth}
    });

    mesh.SetNormals({
        // Front face
        {0,  0,  1 },
        {0,  0,  1 },
        {0,  0,  1 },
        {0,  0,  1 },
        // Back face
        {0,  0,  -1},
        {0,  0,  -1},
        {0,  0,  -1},
        {0,  0,  -1},
        // Top face
        {0,  1,  0 },
        {0,  1,  0 },
        {0,  1,  0 },
        {0,  1,  0 },
        // Bottom face
        {0,  -1, 0 },
        {0,  -1, 0 },
        {0,  -1, 0 },
        {0,  -1, 0 },
        // Right face
        {1,  0,  0 },
        {1,  0,  0 },
        {1,  0,  0 },
        {1,  0,  0 },
        // Left face
        {-1, 0,  0 },
        {-1, 0,  0 },
        {-1, 0,  0 },
        {-1, 0,  0 }
    });

    mesh.SetTexCoords({
        // Front face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Back face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Top face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Bottom face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Right face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Left face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    });

    mesh.SetIndices({// Front
                     0, 1, 2, 2, 3, 0,
                     // Back
                     4, 5, 6, 6, 7, 4,
                     // Top
                     8, 9, 10, 10, 11, 8,
                     // Bottom
                     12, 13, 14, 14, 15, 12,
                     // Right
                     16, 17, 18, 18, 19, 16,
                     // Left
                     20, 21, 22, 22, 23, 20});

    return mesh;
}

} // namespace Object::Utils
