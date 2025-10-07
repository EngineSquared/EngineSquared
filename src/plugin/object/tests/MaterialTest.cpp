#include <gtest/gtest.h>

#include "utils/MaterialBuilder.hpp"

struct Color {
    float r;
    float g;
    float b;
    bool operator==(const Color &other) const = default;
};

using namespace Plugin::Object;

TEST(Material, SetGetData)
{
    Utils::MaterialBuilder mb;

    mb.Set("roughness", 0.5f);
    mb.Set("color", Color{1.0f, 0.0f, 0.0f});

    auto material = mb.Build();

    EXPECT_EQ(material.Get<float>("roughness"), 0.5f);
    EXPECT_EQ(material.Get<Color>("color"), (Color{1.0f, 0.0f, 0.0f}));
    EXPECT_THROW(material.Get<int>("roughness"), MaterialError);
    EXPECT_THROW(material.Get<float>("metallic"), MaterialError);
    material.Set("roughness", 1.0f);
    material.Set("color", Color{0.0f, 1.0f, 0.0f});
    EXPECT_EQ(material.Get<float>("roughness"), 1.0f);
    EXPECT_EQ(material.Get<Color>("color"), (Color{0.0f, 1.0f, 0.0f}));
    EXPECT_THROW(material.Set("unknown", 1.0f), MaterialError);
    EXPECT_THROW(material.Set("color", 1.0f), MaterialError);
}

TEST(Material, ContainsData)
{
    Utils::MaterialBuilder mb;

    mb.Set("color", Color{1.0f, 0.0f, 0.0f});
    mb.Set("roughness", 0.5f);

    auto material = mb.Build();

    EXPECT_TRUE(material.Contains("color"));
    EXPECT_TRUE(material.Contains<Color>("color"));
    EXPECT_FALSE(material.Contains<float>("color"));

    EXPECT_TRUE(material.Contains("roughness"));
    EXPECT_TRUE(material.Contains<float>("roughness"));
    EXPECT_FALSE(material.Contains<Color>("roughness"));

    EXPECT_FALSE(material.Contains("metallic"));
    EXPECT_FALSE(material.Contains<float>("metallic"));
}
