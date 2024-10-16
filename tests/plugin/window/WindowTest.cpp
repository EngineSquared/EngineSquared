#include <gtest/gtest.h>

#include "Window.hpp"

using namespace ES::Plugin::Window;

TEST(Window, launch_window)
{
    Resource::Window window(800, 600, "My Test Window");
    EXPECT_EQ(window.ShouldClose(), false);
}
