#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <stdexcept>

#include "WindowError.hpp"

namespace ES::Plugin::Window::Resource {

/**
 * The Window class is a singleton to handle the creation and destruction of the game's window.
 *
 * @example
 * @code
 * Window window(800, 600, "My Game");
 * while (!window.ShouldClose())
 * {
 *    // Game loop
 * }
 * @endcode
 */
class Window {
  private:
    int _width;
    int _height;
    std::string _title;
    GLFWwindow *_window;
    GLFWmonitor *_monitor;
    GLFWwindow *_share;

  public:
    Window(int width, int height, const std::string &title, GLFWmonitor *monitor = nullptr,
           GLFWwindow *share = nullptr);
    ~Window();

    /**
     * @brief Create and initialize the window.
     *
     * @throws ES::Plugin::Window::Exception::WindowError if the window could not be created.
     */
    void Create();

    /**
     * @brief Check if the window should close.
     *
     * @return True if the window should close, false otherwise.
     */
    inline bool ShouldClose() const { return glfwWindowShouldClose(_window); }

    /**
     * @brief Get a pointer to the GLFW window.
     *
     * @return The GLFW window.
     */
    inline GLFWwindow *GetGLFWWindow() const { return _window; }
};

} // namespace ES::Plugin::Window::Resource
