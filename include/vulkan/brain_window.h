//
// Created by - on 6/12/24.
//

#ifndef BRAIN_WINDOW_H
#define BRAIN_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//~ std
#include <string>

namespace brn {
class BrnWindow {
public:
  BrnWindow(int w, int h, std::string name);
  ~BrnWindow();

  //~ Deleting the copy constructors to prevent mem leak
  BrnWindow(const BrnWindow &) = delete;
  BrnWindow &operator=(const BrnWindow &) = delete;

  //~ Inline
  bool shouldClose() { return glfwWindowShouldClose(window); }
  VkExtent2D getExtent() {
    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
  }
  bool wasWindowResized() { return framebufferResized; }
  void resetWindowResizedFlag() { framebufferResized = false; }

  void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

private:
  //~ Initialize Window
  static void framebufferResizeCallback(GLFWwindow *window, int width,
                                        int height);
  void initWindow();

  //~ Window parameters
  int width;
  int height;
  std::string windowName;

  bool framebufferResized = false;
  //~ Window
  GLFWwindow *window;
};
} // namespace brn
// brn

#endif // brn_WINDOW_H
