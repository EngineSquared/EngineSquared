#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <stdexcept>

// TODO: remove relative path include
#include "../exception/WindowError.hpp"

namespace ES::Plugin::Window::Resource {
    /**
     * The Window class is a singleton to handle the creation and destruction of the game's window.
     */
    class Window
    {
        private:
            int _width;
            int _height;
            std::string _title;
            GLFWwindow* _window;
            GLFWmonitor* _monitor;
            GLFWwindow* _share;

        public:
            Window(int width, int height, const std::string& title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr)
                : _width(width), _height(height), _title(title), _monitor(monitor), _share(share)
            {
            }

            /**
             * @brief Create and initialize the window.
             *
             * @throws ES::Plugin::Window::Exception::WindowError if the window could not be created.
             */
            void Create()
            {
                if (!glfwInit())
                {
                    throw ES::Plugin::Window::Exception::WindowError("Failed to initialize GLFW");
                }

                _window = glfwCreateWindow(_width, _height, _title.c_str(), _monitor, _share);

                if (!_window)
                {
                    glfwTerminate();
                    throw ES::Plugin::Window::Exception::WindowError("Failed to create GLFW window");
                }

                glfwMakeContextCurrent(_window);
            }

            /**
             * @brief Check if the window should close.
             *
             * @return True if the window should close, false otherwise.
             */
            inline bool ShouldClose() const
            {
                return glfwWindowShouldClose(_window);
            }

            /**
             * @brief Get a pointer to the GLFW window.
             *
             * @return The GLFW window.
             */
            inline GLFWwindow* GetGLFWWindow() const
            {
                return _window;
            }

            ~Window()
            {
                if (_window)
                {
                    glfwDestroyWindow(_window);
                    glfwTerminate();
                }
            }
    };
}