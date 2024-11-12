#include <gtest/gtest.h>

#include "String.hpp"

using namespace ES::Plugin::Utils;

TEST(String, Split)
{
    { // One split
        std::string str = "Hello,World";
        std::vector<std::string> result = String::Split(str, ',');
        ASSERT_EQ(result.size(), 2);
        ASSERT_EQ(result[0], "Hello");
        ASSERT_EQ(result[1], "World");
    }
    { // Many split
        std::string str = "Hello,World,!";
        std::vector<std::string> result = String::Split(str, ',');
        ASSERT_EQ(result.size(), 3);
        ASSERT_EQ(result[0], "Hello");
        ASSERT_EQ(result[1], "World");
        ASSERT_EQ(result[2], "!");
    }
    { // No split
        std::string str = "Hello";
        std::vector<std::string> result = String::Split(str, ',');
        ASSERT_EQ(result.size(), 1);
        ASSERT_EQ(result[0], "Hello");
    }
}

TEST(String, EndsWith)
{
    { // Short suffix
        std::string str = "Hello,World";
        std::string suffix = "World";
        ASSERT_TRUE(String::EndsWith(str, suffix));
    }
    { // Invalid suffix
        std::string str = "Hello,World";
        std::string suffix = "World!";
        ASSERT_FALSE(String::EndsWith(str, suffix));
    }
    { // Whole string
        std::string str = "Hello,World";
        std::string suffix = "Hello,World";
        ASSERT_TRUE(String::EndsWith(str, suffix));
    }
    { // Empty suffix
        std::string str = "Hello,World";
        std::string suffix = "";
        ASSERT_TRUE(String::EndsWith(str, suffix));
    }
    { // Empty string with empty suffix
        std::string str = "";
        std::string suffix = "";
        ASSERT_TRUE(String::EndsWith(str, suffix));
    }
    { // Too long suffix
        std::string str = "Hello,World";
        std::string suffix = "!Hello,World";
        ASSERT_FALSE(String::EndsWith(str, suffix));
    }
}
