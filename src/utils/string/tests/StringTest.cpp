#include <gtest/gtest.h>

#include "String.hpp"

using namespace ES::Plugin::Utils;

TEST(Split, OneSplit)
{
    std::string str = "Hello,World";
    std::vector<std::string> result = String::Split(str, ',');
    ASSERT_EQ(result.size(), 2);
    ASSERT_EQ(result[0], "Hello");
    ASSERT_EQ(result[1], "World");
}

TEST(Split, ManySplit)
{
    std::string str = "Hello,World,!";
    std::vector<std::string> result = String::Split(str, ',');
    ASSERT_EQ(result.size(), 3);
    ASSERT_EQ(result[0], "Hello");
    ASSERT_EQ(result[1], "World");
    ASSERT_EQ(result[2], "!");
}

TEST(Split, NoSplit)
{
    std::string str = "Hello";
    std::vector<std::string> result = String::Split(str, ',');
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], "Hello");
}

TEST(EndsWith, ShortSuffix)
{
    std::string str = "Hello,World";
    std::string suffix = "World";
    ASSERT_TRUE(String::EndsWith(str, suffix));
}

TEST(EndsWith, InvalidSuffix)
{
    std::string str = "Hello,World";
    std::string suffix = "World!";
    ASSERT_FALSE(String::EndsWith(str, suffix));
}

TEST(EndsWith, WholeString)
{
    std::string str = "Hello,World";
    std::string suffix = "Hello,World";
    ASSERT_TRUE(String::EndsWith(str, suffix));
}

TEST(EndsWith, EmptySuffix)
{
    std::string str = "Hello,World";
    std::string suffix = "";
    ASSERT_TRUE(String::EndsWith(str, suffix));
}

TEST(EndsWith, EmptyStringWithEmptySuffix)
{
    std::string str = "";
    std::string suffix = "";
    ASSERT_TRUE(String::EndsWith(str, suffix));
}

TEST(EndsWith, TooLongSuffix)
{
    std::string str = "Hello,World";
    std::string suffix = "!Hello,World";
    ASSERT_FALSE(String::EndsWith(str, suffix));
}
