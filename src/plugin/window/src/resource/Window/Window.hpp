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

    bool _isFullscreen = false;
    int _windowedX = 0, _windowedY = 0;
    int _windowedWidth = 0, _windowedHeight = 0;

  public:
    Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor = nullptr,
           GLFWwindow *share = nullptr);

    Window &operator=(const Window &) = delete;

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

    void ToggleFullscreen()
    {
        if (!_window) return;

        if (!_isFullscreen) {
            // Save windowed position and size
            glfwGetWindowPos(_window, &_windowedX, &_windowedY);
            glfwGetWindowSize(_window, &_windowedWidth, &_windowedHeight);

            // Get primary monitor and video mode
            _monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(_monitor);

            // Switch to fullscreen
            glfwSetWindowMonitor(_window, _monitor,
                                0, 0,
                                mode->width, mode->height,
                                mode->refreshRate);
        } else {
            // Restore windowed mode
            glfwSetWindowMonitor(_window, nullptr,
                                _windowedX, _windowedY,
                                _windowedWidth, _windowedHeight,
                                0);
        }

        _isFullscreen = !_isFullscreen;
    }
};

} // namespace ES::Plugin::Window::Resource
