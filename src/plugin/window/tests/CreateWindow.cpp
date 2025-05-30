#include <gtest/gtest.h>

#include "Window.hpp"

using namespace ES::Plugin::Window::Resource;

/**
 * @brief Test case for creating a window with valid parameters.
 *
 * Create a window with null parameters
 * Check if the window is null
 */
TEST(CreateWindowTest, create_null_window)
{
    Window window(0, 0, "", nullptr, nullptr);

    std::cout << "Window: " << window.GetGLFWWindow() << std::endl;
    EXPECT_EQ(window.GetGLFWWindow(), nullptr);
}
