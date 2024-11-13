#include "CollisionUtils3D.hpp"

bool ES::Plugin::Physics::Utils::Box3DCollidesBox3D(const glm::vec3 &posA,
                                                    const ES::Plugin::Physics::Component::BoxCollider3D &boxA,
                                                    const glm::vec3 &posB,
                                                    const ES::Plugin::Physics::Component::BoxCollider3D &boxB)
{
    glm::vec3 boundingBoxAMin = posA;
    glm::vec3 boundingBoxAMax = posA + boxA.size;
    glm::vec3 boundingBoxBMin = posB;
    glm::vec3 boundingBoxBMax = posB + boxB.size;

    return boundingBoxAMax.x >= boundingBoxBMin.x && boundingBoxAMax.y >= boundingBoxBMin.y &&
           boundingBoxAMax.z >= boundingBoxBMin.z && boundingBoxAMin.x <= boundingBoxBMax.x &&
           boundingBoxAMin.y <= boundingBoxBMax.y && boundingBoxAMin.z <= boundingBoxBMax.z;
}
