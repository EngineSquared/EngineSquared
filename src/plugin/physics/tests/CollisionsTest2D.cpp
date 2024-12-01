#include <gtest/gtest.h>

#include "CollisionUtils2D.hpp"

using namespace ES::Plugin::Physics;

TEST(Collision, Point2DCollidesRect2D)
{
    ES::Plugin::Math::Rect rect1{
        {0, 0},
        {1, 1}
    };

    EXPECT_TRUE(Utils::Point2DCollidesRect2D(rect1, {0.5, 0.5}));
    EXPECT_TRUE(Utils::Point2DCollidesRect2D(rect1, {0, 0}));
    EXPECT_TRUE(Utils::Point2DCollidesRect2D(rect1, {1, 1}));
    EXPECT_TRUE(Utils::Point2DCollidesRect2D(rect1, {0, 1}));
    EXPECT_TRUE(Utils::Point2DCollidesRect2D(rect1, {1, 0}));

    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {1.1, 1.1}));
    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {-0.1, -0.1}));
    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {0.5, 1.1}));
    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {1.1, 0.5}));
    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {0.5, -0.1}));
    EXPECT_FALSE(Utils::Point2DCollidesRect2D(rect1, {-0.1, 0.5}));
}
