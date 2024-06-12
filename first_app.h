//
// Created by - on 6/12/24.
//

#ifndef FIRST_APP_H
#define FIRST_APP_H

#include "lve_window.h"
#include "lve_pipeline.h"
#include "lve_device.h"

namespace lve {

class FirstApp {
public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();
private:
    LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
    LveDevice lveDevice{lveWindow};
    LvePipeline lvePipeline{
        lveDevice,
        "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/simple_shader.vert.spv",
        "/Users/c2/Documents/BrainEngine/BrainEngine/shaders/simple_shader.frag.spv",
        LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
};

} // lve

#endif //FIRST_APP_H
