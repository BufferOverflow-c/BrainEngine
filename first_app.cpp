//
// Created by - on 6/12/24.
//

#include "first_app.h"

namespace lve {
    void FirstApp::run() {
        while(!lveWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
} // lve