//
// Created by - on 6/12/24.
//

#include "brain_window.h"

//~ std
#include <stdexcept>

namespace brn {
    BrnWindow::BrnWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    BrnWindow::~BrnWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void BrnWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR*surface) {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }

    void BrnWindow::initWindow() {
        glfwInit();
        //~ Prevents OpenGL Calls
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        //~ Resizing needs to be done by Vulkan
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }
} // brn