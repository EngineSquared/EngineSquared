#include <gtest/gtest.h>

#include "resource/Material.hpp"

struct Color {
    float r, g, b;
    bool operator==(const Color &other) const { return r == other.r && g == other.g && b == other.b; }
};

using namespace Plugin::Object::Resource;

TEST(Material, SetGetData)
{
    Material material;

    material.Set("color", Color{1.0f, 0.0f, 0.0f});
    material.Set("roughness", 0.5f);

    EXPECT_EQ(material.Get<Color>("color"), (Color{1.0f, 0.0f, 0.0f}));
    EXPECT_EQ(material.Get<float>("roughness"), 0.5f);
}

TEST(Material, ContainsData)
{
    Material material;

    material.Set("color", Color{1.0f, 0.0f, 0.0f});
    material.Set("roughness", 0.5f);

    EXPECT_TRUE(material.Contains("color"));
    EXPECT_TRUE(material.Contains<Color>("color"));
    EXPECT_FALSE(material.Contains<float>("color"));

    EXPECT_TRUE(material.Contains("roughness"));
    EXPECT_TRUE(material.Contains<float>("roughness"));
    EXPECT_FALSE(material.Contains<Color>("roughness"));

    EXPECT_FALSE(material.Contains("metallic"));
    EXPECT_FALSE(material.Contains<float>("metallic"));
}
