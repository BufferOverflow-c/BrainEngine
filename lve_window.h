//
// Created by - on 6/12/24.
//

#ifndef LVE_WINDOW_H
#define LVE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//~ std
#include <string>

namespace lve {
    class LveWindow {
    public:
        LveWindow(int w, int h, std::string name);
        ~LveWindow();

        //~ Deleting the copy constructors to prevent mem leak
        LveWindow(const LveWindow &) = delete;
        LveWindow &operator=(const LveWindow &) = delete;

        //~ Inline
        bool shouldClose() { return glfwWindowShouldClose(window); }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        //~ Initialize Window
        void initWindow();

        //~ Window parameters
        const int width;
        const int height;
        std::string windowName;
        //~ Window
        GLFWwindow *window;
    };
}
// lve

#endif //LVE_WINDOW_H
