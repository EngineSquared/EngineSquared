#include "Window.hpp"
#include "Logger.hpp"

namespace ES::Plugin::Window::Resource {

Window::Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share)
    : _title(title), _window(nullptr), _monitor(monitor), _share(share)
{
    _window = glfwCreateWindow(width, height, _title.c_str(), _monitor, _share);
    if (!_window)
        ES::Utils::Log::Error("Failed to create window");
}

void Window::Destroy()
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    /* To use in a shutdown scheduler */
    glfwDestroyWindow(_window);
}

glm::ivec2 Window::GetSize()
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glm::ivec2 size;
    glfwGetWindowSize(_window, &size.x, &size.y);
    return size;
}

void Window::SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback)
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glfwSetWindowUserPointer(_window, userPointer);
    glfwSetFramebufferSizeCallback(_window, callback);
}

void Window::SetSize(int width, int height) { glfwSetWindowSize(_window, width, height); }

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
