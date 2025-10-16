#include <gtest/gtest.h>

#include "component/Transform.hpp"

using namespace Object;

TEST(Transform, struct_size_comparaison)
{
    Component::Transform transform;
    transform.SetPosition(1.0f, 2.0f, 3.0f);
    transform.SetScale(4.0f, 5.0f, 6.0f);
    transform.SetRotation(7.0f, 8.0f, 9.0f, 10.0f);

    EXPECT_EQ((sizeof(glm::vec3(1)) + sizeof(glm::vec3(1)) + sizeof(glm::quat(1, 2, 3, 4))), sizeof(transform));
}
