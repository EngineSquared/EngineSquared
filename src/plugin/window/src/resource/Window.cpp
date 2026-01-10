#include "Window.hpp"
#include "Logger.hpp"

namespace Window::Resource {

Window::Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share)
    : _title(title), _window(nullptr), _monitor(monitor), _share(share)
{
    _window = glfwCreateWindow(width, height, _title.c_str(), _monitor, _share);
    if (!_window)
        Log::Error("Failed to create window");
}

void Window::Destroy()
{
    if (!_window)
        throw Exception::WindowError("Window is not created");

    /* To use in a shutdown scheduler */
    glfwDestroyWindow(_window);
}

glm::uvec2 Window::GetSize() const
{
    glm::ivec2 size;
    glfwGetWindowSize(_window, &size.x, &size.y);

    return glm::uvec2(size);
}

void Window::SetSize(int width, int height) { glfwSetWindowSize(_window, width, height); }

void Window::ToggleFullscreen()
{
    if (!_window)
        return;

    if (!_isFullscreen)
    {
        glfwGetWindowPos(_window, &_windowedX, &_windowedY);
        glfwGetWindowSize(_window, &_windowedWidth, &_windowedHeight);

        _monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(_monitor);

        glfwSetWindowMonitor(_window, _monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        glfwSetWindowMonitor(_window, nullptr, _windowedX, _windowedY, _windowedWidth, _windowedHeight, 0);
    }

    _isFullscreen = !_isFullscreen;
}

} // namespace Window::Resource
