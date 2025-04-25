#include "Window.hpp"
#include "Logger.hpp"

namespace ES::Plugin::Window::Resource {

Window::Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share)
    : _size({width, height}), _title(title), _window(nullptr), _monitor(monitor), _share(share)
{
    _window = glfwCreateWindow(_size.x, _size.y, _title.c_str(), _monitor, _share);
#ifdef ES_DEBUG
    if (!_window)
        ES::Utils::Error("Failed to create window");
#endif
}

void Window::Destroy()
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    /* To use in a shutdown scheduler */
    glfwDestroyWindow(_window);
}

glm::ivec2 &Window::GetSize()
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glfwGetWindowSize(_window, &_size.x, &_size.y);
    return _size;
}

void Window::SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback)
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glfwSetWindowUserPointer(_window, userPointer);
    glfwSetFramebufferSizeCallback(_window, callback);
}

void Window::SetSize(int width, int height)
{
    _size = {width, height};
    glfwSetWindowSize(_window, _size.x, _size.y);
}

void Window::ToggleFullscreen()
{
    if (!_window)
        return;

    if (!_isFullscreen)
    {
        // Save windowed position and size
        glfwGetWindowPos(_window, &_windowedX, &_windowedY);
        glfwGetWindowSize(_window, &_windowedWidth, &_windowedHeight);

        // Get primary monitor and video mode
        _monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(_monitor);

        // Switch to fullscreen
        glfwSetWindowMonitor(_window, _monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        // Restore windowed mode
        glfwSetWindowMonitor(_window, nullptr, _windowedX, _windowedY, _windowedWidth, _windowedHeight, 0);
    }

    _isFullscreen = !_isFullscreen;
}

} // namespace ES::Plugin::Window::Resource
