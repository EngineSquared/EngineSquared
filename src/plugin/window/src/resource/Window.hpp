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
 *    GameLoop();
 * }
 * @endcode
 */
class Window {
  private:
    uint32_t _width;
    uint32_t _height;
    std::string _title;
    GLFWwindow *_window;
    GLFWmonitor *_monitor;
    GLFWwindow *_share;

  public:
    Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor = nullptr,
           GLFWwindow *share = nullptr);

    Window &operator=(const Window &) = delete;

    /**
     * @brief Create and initialize the window.
     *
     * @throws ES::Plugin::Window::Exception::WindowError if the window could not be created.
     */
    void Create();

    /**
     * @brief Destroy the window.
     */
    void Destroy();

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

    /**
     * @brief Get the window size.
     *
     * @param[out] width The width of the window.
     * @param[out] height The height of the window.
     */
    void GetWindowSize(int &width, int &height);

    /**
     * @brief Set the framebuffer size callback.
     *
     * @param userPointer The user pointer.
     * @param callback The callback function.
     */
    void SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback);
};

} // namespace ES::Plugin::Window::Resource
