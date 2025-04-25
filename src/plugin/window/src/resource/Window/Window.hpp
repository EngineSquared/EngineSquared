#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
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
    glm::ivec2 _size;
    std::string _title;
    GLFWwindow *_window;
    GLFWmonitor *_monitor;
    GLFWwindow *_share;

    bool _isFullscreen = false;
    int _windowedX = 0;
    int _windowedY = 0;
    int _windowedWidth = 0;
    int _windowedHeight = 0;

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
     * @brief Set the window title.
     *
     * @param title The new title of the window.
     */
    inline void SetTitle(std::string_view title)
    {
        _title = title;
        glfwSetWindowTitle(_window, _title.c_str());
    }

    /**
     * @brief Get the window size.
     *
     * This function will update the window size from the GLFW window content area and return it.
     *
     * @return A vector of integers to store the size of the window.
     */
    glm::ivec2 &GetSize();

    /**
     * @brief Set the framebuffer size callback.
     *
     * @param userPointer The user pointer.
     * @param callback The callback function.
     */
    void SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback);

    void SetSize(int width, int height);

    void ToggleFullscreen();

    /**
     * @brief Get the current mouse position.
     *
     * @return A pair containing the x and y coordinates of the mouse.
     */
    inline glm::vec2 GetMousePosition()
    {
        double x = 0;
        double y = 0;
        glm::ivec2 windowSize = GetSize();
        glfwGetCursorPos(_window, &x, &y);
        y = windowSize.y - y;
        return {x, y};
    }
};

} // namespace ES::Plugin::Window::Resource
