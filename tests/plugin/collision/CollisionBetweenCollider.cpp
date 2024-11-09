#include <gtest/gtest.h>

#include "CollisionBetweenCollider.hpp"

using namespace ES::Plugin::Collision;

TEST(Collision, CollidePointRect)
{
    ES::Plugin::Math::Rect rect1({0, 0}, {1, 1});

    EXPECT_TRUE(Utils::CollidePointRect(rect1, {0.5, 0.5}));
    EXPECT_TRUE(Utils::CollidePointRect(rect1, {0, 0}));
    EXPECT_TRUE(Utils::CollidePointRect(rect1, {1, 1}));
    EXPECT_TRUE(Utils::CollidePointRect(rect1, {0, 1}));
    EXPECT_TRUE(Utils::CollidePointRect(rect1, {1, 0}));

    EXPECT_FALSE(Utils::CollidePointRect(rect1, {1.1, 1.1}));
    EXPECT_FALSE(Utils::CollidePointRect(rect1, {-0.1, -0.1}));
    EXPECT_FALSE(Utils::CollidePointRect(rect1, {0.5, 1.1}));
    EXPECT_FALSE(Utils::CollidePointRect(rect1, {1.1, 0.5}));
    EXPECT_FALSE(Utils::CollidePointRect(rect1, {0.5, -0.1}));
    EXPECT_FALSE(Utils::CollidePointRect(rect1, {-0.1, 0.5}));
}
