//
// Created by - on 6/12/24.
//

#include "brain_window.h"

//~ std
#include <stdexcept>

namespace brn {
BrnWindow::BrnWindow(int w, int h, std::string name)
    : width{w}, height{h}, windowName{name} {
  initWindow();
}

BrnWindow::~BrnWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}

void BrnWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
  if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface");
  }
}

void BrnWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
  auto brnWindow = reinterpret_cast<BrnWindow *>(glfwGetWindowUserPointer(window));
  brnWindow->framebufferResized = true;
  brnWindow->width = width;
  brnWindow->height = height;
}

void BrnWindow::initWindow() {
  glfwInit();
  //~ Prevents OpenGL Calls
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  //~ Resizing needs to be done by Vulkan
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  glfwSetWindowUserPointer(window, this);
  glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}
} // namespace brn
