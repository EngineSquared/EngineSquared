#include <gtest/gtest.h>

#include "Transform.hpp"

using namespace ES::Plugin::Object;

TEST(Transform, struct_size_comparaison)
{
    Component::Transform transform;
    transform.setPosition(1.0f, 2.0f, 3.0f);
    transform.setScale(4.0f, 5.0f, 6.0f);
    transform.setRotation(7.0f, 8.0f, 9.0f, 10.0f);

    EXPECT_EQ((sizeof(glm::vec3(1)) + sizeof(glm::vec3(1)) + sizeof(glm::quat(1, 2, 3, 4))), sizeof(transform));

    uint8_t *data = reinterpret_cast<uint8_t *>(&transform);
    Component::Transform *reserialized_data = reinterpret_cast<Component::Transform *>(data);
    EXPECT_EQ(reserialized_data->getPosition(), transform.getPosition());
    EXPECT_EQ(reserialized_data->getScale(), transform.getScale());
    EXPECT_EQ(reserialized_data->getRotation(), transform.getRotation());
}
